/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_exec.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:18:06 by rafael            #+#    #+#             */
/*   Updated: 2026/04/25 05:32:51 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <http/cgi/CGI.hpp>
#include <http/routing/Router.hpp>
#include <signal.h>
#include <unistd.h>
#include <utils/http/mime.hpp>

bool CGI::create_Pipes(int inPipe[2], int outPipe[2])
{
	if (pipe(inPipe) == -1)
	{
		std::cerr << "Error creating inPipe" << std::endl;
		return (false);
	}
	if (pipe(outPipe) == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		std::cerr << "Error creating outPipe" << std::endl;
		return (false);
	}
	return (true);
}

static std::string get_CgiPath(const std::map<std::string,
	std::string> &cgi_Path, std::string &filename)
{
	std::string extension = get_Extension(filename);
	std::map<std::string,
		std::string>::const_iterator it = cgi_Path.find(extension);
	if (it != cgi_Path.end())
		return (it->second);
	return ("");
}

void CGI::execute_ChildProcess(int inPipe[2], int outPipe[2],
	const std::string &scriptPath, const std::map<std::string,
	std::string> &cgiPath, char *const envp[])
{
	std::string dir = scriptPath.substr(0, scriptPath.find_last_of('/') + 1);
	if (chdir(dir.c_str()) == -1)
	{
		perror("chdir failed");
		exit(1);
	}
	std::string filename = scriptPath.substr(scriptPath.find_last_of('/') + 1);
	std::string interpreter = get_CgiPath(cgiPath, filename);
	if (interpreter.empty())
		exit(1);
	close(inPipe[1]);
	close(outPipe[0]);
	if (dup2(inPipe[0], STDIN_FILENO) == -1)
	{
		close(inPipe[0]);
		close(outPipe[1]);
		exit(1);
	}
	close(inPipe[0]);
	if (dup2(outPipe[1], STDOUT_FILENO) == -1)
	{
		close(outPipe[1]);
		exit(1);
	}
	close(outPipe[1]);
	std::vector<char *> argv = build_Arguments(filename, interpreter);
	execve(interpreter.c_str(), &argv[0], envp);
	exit(1);
}

/*
** launch() — non-blocking CGI startup.
**
** Validates the script, creates pipes, forks, sets the parent-side
** pipe ends to O_NONBLOCK, and populates ctx.
**
** The child process is set up synchronously (execute_ChildProcess handles
** that entirely before execve).  Only the parent-side descriptors are made
** non-blocking; the child inherits the blocking ends, closes them after
** dup2, and never sees O_NONBLOCK.
**
** On success: ctx.active == true, ctx.inFd and ctx.outFd are valid
** O_NONBLOCK file descriptors ready to be registered in the main poll().
**
** On failure: all fds are closed, ctx.active remains false, returns false.
*/
int CGI::launch(const Request &req, Location &location, CgiContext &ctx)
{
	int		inPipe[2];
	int		outPipe[2];
	pid_t	pid;
	int		flags;

	std::string scriptPath = resolve_ScriptPath(req.get_Path());
	if (!is_acceptableExtension(req.get_Path(), location))
		return (403);
	if (!is_InsideRoot(scriptPath, router->get_Config().config.root))
		return (403);
	if (!check_File(scriptPath))
		return (404);
	if (!is_Executable(scriptPath))
		return (403);
	build_Environment(req, scriptPath);
	std::vector<char *> envp = convert_Env(env);
	if (!create_Pipes(inPipe, outPipe))
		return (500);
	pid = fork();
	if (pid == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		return (500);
	}
	if (pid == 0)
	{
		// Child: blocking descriptors, exec, never returns
		execute_ChildProcess(inPipe, outPipe, scriptPath, location.cgiPath,
			&envp[0]);
		exit(1); // unreachable but defensive
	}
	// Parent: close child-side ends
	close(inPipe[0]);
	close(outPipe[1]);
	// Set parent-side ends non-blocking so they can participate in the
	// single main poll() without ever blocking the event loop.
	flags = fcntl(inPipe[1], F_GETFL, 0);
	if (flags == -1 || fcntl(inPipe[1], F_SETFL, flags | O_NONBLOCK) == -1)
	{
		close(inPipe[1]);
		close(outPipe[0]);
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		return (500);
	}
	flags = fcntl(outPipe[0], F_GETFL, 0);
	if (flags == -1 || fcntl(outPipe[0], F_SETFL, flags | O_NONBLOCK) == -1)
	{
		close(inPipe[1]);
		close(outPipe[0]);
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		return (500);
	}
	// Populate context
	ctx.active = true;
	ctx.pid = pid;
	ctx.inFd = inPipe[1];
	ctx.outFd = outPipe[0];
	ctx.bodyToSend = req.get_Body();
	ctx.bodyOffset = 0;
	ctx.output.clear();
	ctx.startTime = time(NULL);
	return (0);
}

/*
** finish() — called after EOF on outPipe[0].
**
** outFd must already be closed by the caller (Server) before calling
** this function so that ctx.outFd == -1.
**
** Parses the accumulated CGI output and builds the HTTP Response.
** If the CGI exited abnormally or the output is invalid, an error
** response is returned.
*/
Response CGI::finish(CgiContext &ctx, int waitStatus)
{
	CGIResult	result;
	Response	res;

	if (!WIFEXITED(waitStatus) || WEXITSTATUS(waitStatus) != 0)
		return (router->make_ErrorCode(500));
	if (!is_ValidCGIOutput(ctx.output))
		return (router->make_ErrorCode(502));
	result = parse_CGIOutput(ctx.output);
	res.set_StatusCode(result.status);
	res.set_Header("Content-Type", result.contentType);
	if (result.headers.count("Location"))
		res.set_Header("Location", result.headers.at("Location"));
	res.set_Body(result.body);
	return res;
}
