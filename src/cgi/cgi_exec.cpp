/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_exec.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:18:06 by rafael            #+#    #+#             */
/*   Updated: 2026/03/27 02:58:58 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <http/cgi/CGI.hpp>

void CGI::create_Pipes(int inPipe[2], int outPipe[2])
{
	if (pipe(inPipe) == -1)
	{
		std::cerr << "Error creating inPipe" << std::endl;
		return ;
	}
	if (pipe(outPipe) == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		std::cerr << "Error creating inPipe" << std::endl;
		return ;
	}
}

void CGI::execute_ChildProcess(int inPipe[2], int outPipe[2],
	const std::string &scriptPath, char *const argv[], char *const envp[])
{
	close(inPipe[1]);
	close(outPipe[0]);
	if (dup2(inPipe[0], STDIN_FILENO) == -1)
	{
		close(inPipe[0]);
		close(outPipe[1]);
		exit(1);
	}
	if (dup2(outPipe[1], STDOUT_FILENO) == -1)
	{
		close(inPipe[0]);
		close(outPipe[1]);
		exit(1);
	}
	if (execve(scriptPath.c_str(), argv, envp) == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		exit(1);
	}
	//exit(1);
}

std::string CGI::handle_ParentProcess(int inPipe[2], int outPipe[2], pid_t pid, int &status,
	const Request &req)
{
    std::string buff;
    int bytes;
	int elapsed = 0;
    char temp[4096];
	close(inPipe[0]);
	close(outPipe[1]);
    if (!req.get_Body().empty())
	    write(inPipe[1], req.get_Body().c_str(), req.get_Body().size());
	close(inPipe[1]);
	fcntl(outPipe[0], F_SETFL, O_NONBLOCK);
	while (true)
    {
        bytes = read(outPipe[0], temp, sizeof(temp));
        if (bytes > 0)
        {
            if (buff.size() + bytes > MAX_CGI_OUTPUT)
            {
				std::cout << "aqui3\n";
                kill(pid, SIGKILL);
                waitpid(pid, NULL, 0);
                return "";
            }
            buff.append(temp, bytes);
        }
        else if (bytes == 0)
		{
            break; // EOF → processo terminou output
		}
		pid_t result = waitpid(pid, &status, WNOHANG);
		if (result == pid)
			break;
		if (elapsed >= 50)
		{
			kill(pid, SIGKILL);
			waitpid(pid, &status, 0);
			break;
		}
		usleep(100000);
		elapsed++;
	}
    close(outPipe[0]);
    return buff;
}
