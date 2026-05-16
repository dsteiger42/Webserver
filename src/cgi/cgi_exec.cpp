/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_exec.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:18:06 by rafael            #+#    #+#             */
/*   Updated: 2026/05/16 03:46:31 by rafael           ###   ########.fr       */
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

	std::cerr << "[CGI DEBUG] get_CgiPath: filename='" << filename
	          << "' extension='" << extension << "'" << std::endl;
	std::cerr << "[CGI DEBUG] get_CgiPath: cgiPath tem "
	          << cgi_Path.size() << " entradas:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it2 = cgi_Path.begin();
	     it2 != cgi_Path.end(); ++it2)
		std::cerr << "[CGI DEBUG]   '" << it2->first
		          << "' => '" << it2->second << "'" << std::endl;

	std::map<std::string,
		std::string>::const_iterator it = cgi_Path.find(extension);
	if (it != cgi_Path.end())
	{
		std::cerr << "[CGI DEBUG] get_CgiPath: interpreter='"
		          << it->second << "'" << std::endl;
		return (it->second);
	}
	std::cerr << "[CGI DEBUG] get_CgiPath: NENHUM interpreter para '"
	          << extension << "'" << std::endl;
	return ("");
}

static void debug_write(const char *msg)
{
	write(STDERR_FILENO, msg, strlen(msg));
}

void CGI::execute_ChildProcess(int inPipe[2], int outPipe[2],
	const std::string &scriptPath, const std::map<std::string,
	std::string> &cgiPath, char *const envp[])
{
	debug_write("[CGI DEBUG FILHO] execute_ChildProcess: inicio\n");

	std::string dir = scriptPath.substr(0, scriptPath.find_last_of('/') + 1);
	std::string filename = scriptPath.substr(scriptPath.find_last_of('/') + 1);

	std::string msg;
	msg = "[CGI DEBUG FILHO] scriptPath='" + scriptPath + "'\n";
	debug_write(msg.c_str());
	msg = "[CGI DEBUG FILHO] dir='" + dir + "'\n";
	debug_write(msg.c_str());
	msg = "[CGI DEBUG FILHO] filename='" + filename + "'\n";
	debug_write(msg.c_str());

	if (chdir(dir.c_str()) == -1)
	{
		perror("[CGI DEBUG FILHO] chdir FALHOU");
		exit(1);
	}
	debug_write("[CGI DEBUG FILHO] chdir OK\n");

	std::string interpreter = get_CgiPath(cgiPath, filename);
	if (interpreter.empty())
	{
		debug_write("[CGI DEBUG FILHO] interpreter vazio - extensao nao mapeada\n");
		exit(1);
	}
	msg = "[CGI DEBUG FILHO] interpreter='" + interpreter + "'\n";
	debug_write(msg.c_str());

	close(inPipe[1]);
	close(outPipe[0]);
	debug_write("[CGI DEBUG FILHO] pipes pai fechados\n");

	if (dup2(inPipe[0], STDIN_FILENO) == -1)
	{
		perror("[CGI DEBUG FILHO] dup2 STDIN FALHOU");
		close(inPipe[0]);
		close(outPipe[1]);
		exit(1);
	}
	close(inPipe[0]);
	debug_write("[CGI DEBUG FILHO] dup2 STDIN OK\n");

	if (dup2(outPipe[1], STDOUT_FILENO) == -1)
	{
		perror("[CGI DEBUG FILHO] dup2 STDOUT FALHOU");
		close(outPipe[1]);
		exit(1);
	}
	close(outPipe[1]);
	debug_write("[CGI DEBUG FILHO] dup2 STDOUT OK\n");

	msg = "[CGI DEBUG FILHO] execve: interpreter='" + interpreter
	      + "' argv[1]='" + filename + "'\n";
	debug_write(msg.c_str());

	std::vector<char *> argv = build_Arguments(filename, interpreter);
	debug_write("[CGI DEBUG FILHO] a chamar execve...\n");
	execve(interpreter.c_str(), &argv[0], envp);
	perror("[CGI DEBUG FILHO] execve FALHOU");
	exit(1);
}

int CGI::launch(const Request &req, Location &location, CgiContext &ctx, unsigned long tick)
{
	int		inPipe[2];
	int		outPipe[2];
	pid_t	pid;

	std::cerr << "[CGI DEBUG] launch: path='" << req.get_Path()
	          << "' method='" << req.get_Method() << "'" << std::endl;

	std::string scriptPath = resolve_ScriptPath(req.get_Path());
	std::cerr << "[CGI DEBUG] launch: scriptPath='" << scriptPath << "'" << std::endl;

	if (!is_acceptableExtension(req.get_Path(), location))
	{
		std::cerr << "[CGI DEBUG] launch: is_acceptableExtension FALHOU" << std::endl;
		return (403);
	}
	std::cerr << "[CGI DEBUG] launch: is_acceptableExtension OK" << std::endl;

	std::string DocumentRoot;
	if (location.root.empty())
		DocumentRoot = router->get_Config().config.root;
	else
		DocumentRoot = location.root;
	std::cerr << "[CGI DEBUG] launch: DocumentRoot='" << DocumentRoot << "'" << std::endl;

	if (!is_InsideRoot(scriptPath, DocumentRoot))
	{
		std::cerr << "[CGI DEBUG] launch: is_InsideRoot FALHOU" << std::endl;
		return (403);
	}
	std::cerr << "[CGI DEBUG] launch: is_InsideRoot OK" << std::endl;

	if (!check_File(scriptPath))
	{
		std::cerr << "[CGI DEBUG] launch: check_File FALHOU - nao existe: '"
		          << scriptPath << "'" << std::endl;
		return (404);
	}
	std::cerr << "[CGI DEBUG] launch: check_File OK" << std::endl;

	if (!is_Executable(scriptPath))
	{
		std::cerr << "[CGI DEBUG] launch: is_Executable FALHOU" << std::endl;
		return (403);
	}
	std::cerr << "[CGI DEBUG] launch: is_Executable OK" << std::endl;

	build_Environment(req, scriptPath);
	std::cerr << "[CGI DEBUG] launch: environment (" << env.size() << " vars):" << std::endl;
	for (size_t i = 0; i < env.size(); i++)
		std::cerr << "[CGI DEBUG]   " << env[i] << std::endl;

	std::vector<char *> envp = convert_Env(env);
	if (!create_Pipes(inPipe, outPipe))
		return (500);

	pid = fork();
	if (pid == -1)
	{
		std::cerr << "[CGI DEBUG] launch: fork() FALHOU" << std::endl;
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		return (500);
	}
	if (pid == 0)
	{
		execute_ChildProcess(inPipe, outPipe, scriptPath, location.cgiPath,
			&envp[0]);
		exit(1);
	}
	std::cerr << "[CGI DEBUG] launch: filho pid=" << pid << std::endl;

	close(inPipe[0]);
	close(outPipe[1]);
	if (fcntl(inPipe[1], F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "[CGI DEBUG] launch: fcntl inPipe O_NONBLOCK FALHOU" << std::endl;
		close(inPipe[1]);
		close(outPipe[0]);
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		return (500);
	}
	if (fcntl(outPipe[0], F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "[CGI DEBUG] launch: fcntl outPipe O_NONBLOCK FALHOU" << std::endl;
		close(inPipe[1]);
		close(outPipe[0]);
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
		return (500);
	}
	ctx.active = true;
	ctx.pid = pid;
	ctx.inFd = inPipe[1];
	ctx.outFd = outPipe[0];
	ctx.bodyToSend = req.get_Body();
	ctx.bodyOffset = 0;
	ctx.output.clear();
	ctx.startTime  = tick;
	std::cerr << "[CGI DEBUG] launch: OK - inFd=" << ctx.inFd
	          << " outFd=" << ctx.outFd << std::endl;
	return (0);
}


Response CGI::finish(CgiContext &ctx, int waitStatus)
{
	CGIResult	result;
	Response	res;

	std::cerr << "[CGI DEBUG] finish: waitStatus=" << waitStatus << std::endl;

	if (!WIFEXITED(waitStatus))
	{
		if (WIFSIGNALED(waitStatus))
			std::cerr << "[CGI DEBUG] finish: filho morto por SINAL="
			          << WTERMSIG(waitStatus) << std::endl;
		else
			std::cerr << "[CGI DEBUG] finish: filho nao terminou normalmente" << std::endl;
		return (router->make_ErrorCode(500));
	}
	int exitCode = WEXITSTATUS(waitStatus);
	std::cerr << "[CGI DEBUG] finish: exit code=" << exitCode << std::endl;
	if (exitCode != 0)
	{
		std::cerr << "[CGI DEBUG] finish: exit code != 0 => 500" << std::endl;
		return (router->make_ErrorCode(500));
	}
	std::cerr << "[CGI DEBUG] finish: output=" << ctx.output.size() << " bytes" << std::endl;
	if (!ctx.output.empty())
		std::cerr << "[CGI DEBUG] finish: output[0..200]='"
		          << ctx.output.substr(0, 500) << "'" << std::endl;
	if (ctx.output.empty())
	{
		std::cerr << "[CGI DEBUG] finish: output vazio => 200 vazio" << std::endl;
		res.set_StatusCode(200);
		res.set_Header("Content-Type", "text/plain");
		res.set_Body("");
		return res;
	}
	if (!is_ValidCGIOutput(ctx.output))
	{
		std::cerr << "[CGI DEBUG] finish: is_ValidCGIOutput FALHOU => 502" << std::endl;
		return (router->make_ErrorCode(502));
	}
	result = parse_CGIOutput(ctx.output);
	res.set_StatusCode(result.status);
	res.set_Header("Content-Type", result.contentType);
	if (result.headers.count("Location"))
		res.set_Header("Location", result.headers.at("Location"));
	res.set_Body(result.body);
	std::cerr << "[CGI DEBUG] finish: OK status=" << result.status
	          << " content-type='" << result.contentType << "'" << std::endl;
	return res;
}