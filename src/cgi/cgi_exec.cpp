/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_exec.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:18:06 by rafael            #+#    #+#             */
/*   Updated: 2026/04/24 03:09:19 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <http/cgi/CGI.hpp>
#include <poll.h>
#include <utils/http/mime.hpp>
#include <http/routing/Router.hpp>

bool CGI::create_Pipes(int inPipe[2], int outPipe[2])
{
	if (pipe(inPipe) == -1)
	{
		std::cerr << "Error creating inPipe" << std::endl;
		return false;
	}
	if (pipe(outPipe) == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		std::cerr << "Error creating inPipe" << std::endl;
		return false;
	}
	return true;
}

static std::string get_CgiPath(const std::map<std::string, std::string> &cgi_Path, std::string &filename)
{
	std::string extension = get_Extension(filename);
	std::map<std::string,
		std::string>::const_iterator it = cgi_Path.find(extension);
	if (it != cgi_Path.end())
		return it->second;
	return "";
}

void CGI::execute_ChildProcess(int inPipe[2], int outPipe[2],
	const std::string &scriptPath, const std::map<std::string, std::string> &cgiPath, char *const envp[])
{
	std::vector<char *> argv;
	std::string dir = scriptPath.substr(0, scriptPath.find_last_of('/') + 1);
	if (chdir(dir.c_str()) == -1)
	{
		perror("chdir failed");
		exit(1);
	}
	std::string filename = scriptPath.substr(scriptPath.find_last_of('/') + 1);
	std::string extension = get_CgiPath(cgiPath, filename);
	if (extension.empty())
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
	argv = build_Arguments(filename, extension);
	if (execve(extension.c_str(), &argv[0], envp) == -1)
		exit(1);
	exit(1);
}

CGIStartResult CGI::start(const Request &req, Location &location)
{
    CGIStartResult result;
    result.started = false;

    // Validações (inalteradas)
    std::string scriptPath = resolve_ScriptPath(req.get_Path());
    if (!is_acceptableExtension(req.get_Path(), location))
	{ 
		result.errorResponse = router->make_ErrorCode(404); 
		return result; 
	}
    if (!is_InsideRoot(scriptPath, router->get_DocumentRoot()))
    {
		result.errorResponse = router->make_ErrorCode(403);
		return result;
	}
    if (!check_File(scriptPath))
    {
		result.errorResponse = router->make_ErrorCode(404);
		return result;
	}
    if (!is_Executable(scriptPath))
    {
		result.errorResponse = router->make_ErrorCode(403); 
		return result;
	}
    build_Environment(req, scriptPath);
    std::vector<char *> envp = convert_Env(env);
    int inPipe[2], outPipe[2];
    if (!create_Pipes(inPipe, outPipe))
    {
		result.errorResponse = router->make_ErrorCode(500);
		return result; 
	}
    // O_NONBLOCK apenas nos extremos do PAI (inPipe[1] e outPipe[0])
    // O filho usa inPipe[0] e outPipe[1] — esses ficam blocking, correcto
    int flags;
    flags = fcntl(inPipe[1],  F_GETFL, 0); fcntl(inPipe[1],  F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(outPipe[0], F_GETFL, 0); fcntl(outPipe[0], F_SETFL, flags | O_NONBLOCK);
    pid_t pid = fork();
    if (pid == -1)
    {
        close(inPipe[0]);  close(inPipe[1]);
        close(outPipe[0]); close(outPipe[1]);
        result.errorResponse = router->make_ErrorCode(500);
        return result;
    }
    if (pid == 0)
        execute_ChildProcess(inPipe, outPipe, scriptPath, location.cgiPath, &envp[0]);
    // Pai — fechar extremos do filho
    close(inPipe[0]);
    close(outPipe[1]);
    // Preencher CGIPending
    result.started             = true;
    result.pending.pid         = pid;
    result.pending.outPipeFd   = outPipe[0];
    result.pending.active      = true;
    result.pending.bodyWritten = 0;
    result.pending.waitStatus  = 0;
    if (!req.get_Body().empty())
    {
        result.pending.inPipeFd    = inPipe[1];
        result.pending.bodyToWrite = req.get_Body();
    }
    else
    {
        // Sem body → fechar stdin do CGI imediatamente
        close(inPipe[1]);
        result.pending.inPipeFd = -1;
    }
    return result;
}