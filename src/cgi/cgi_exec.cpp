/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_exec.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:18:06 by rafael            #+#    #+#             */
/*   Updated: 2026/03/25 20:26:47 by rafael           ###   ########.fr       */
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
		return ;
	}
	if (dup2(outPipe[1], STDOUT_FILENO) == -1)
	{
		close(inPipe[0]);
		close(outPipe[1]);
		return ;
	}
	if (execve(scriptPath.c_str(), argv, envp) == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		exit(1);
	}
	// exit(1);
}

std::string CGI::handle_ParentProcess(int inPipe[2], int outPipe[2], const Request &req)
{
	size_t	bytes;
	char	temp[4096];

	std::string buff;
	close(inPipe[0]);
	close(outPipe[1]);
	if (!req.get_Body().empty())
		write(inPipe[1], req.get_Body().c_str(), req.get_Body().size());
	close(inPipe[1]);
	while ((bytes = read(outPipe[0], temp, sizeof(temp))) > 0)
	{
		buff.append(temp, bytes);
	}
	close(outPipe[0]);
	return (buff);
}
