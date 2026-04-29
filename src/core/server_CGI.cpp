/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_CGI.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 03:40:51 by rafael            #+#    #+#             */
/*   Updated: 2026/04/29 23:36:56 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <core/server.hpp>

bool Server::start_Cgi(Client &client, const Request &req,
	std::vector<pollfd> &fds, unsigned long tick)
{
	int	result;

	CgiContext &ctx = client.cgi;
	Location &loc = _router.matchLocation(req.get_Path());
	result = _router.cgi->launch(req, loc, ctx, tick);
	if (result != 0)
	{
		client.response = _router.make_ErrorCode(result);
		std::string raw = client.response.serialize();
		client.writeBuffer.write(raw.c_str(), raw.size());
		return (false);
	}
	if (!ctx.bodyToSend.empty())
	{
		add_PollFd(fds, ctx.inFd, POLLOUT);
		_pipeToClient[ctx.inFd] = client.fd;
	}
	else
	{
		close(ctx.inFd);
		ctx.inFd = -1;
	}
	add_PollFd(fds, ctx.outFd, POLLIN);
	_pipeToClient[ctx.outFd] = client.fd;
	return (true);
}

void Server::remove_PipeFd(std::vector<pollfd> &fds, int fd, bool doClose)
{
	for (size_t i = 0; i < fds.size(); i++)
	{
		if (fds[i].fd == fd)
		{
			fds.erase(fds.begin() + i);
			break ;
		}
	}
	_pipeToClient.erase(fd);
	if (doClose)
		close(fd);
}

void Server::abort_Cgi(Client &client, std::vector<pollfd> &fds)
{
	CgiContext &ctx = client.cgi;
	if (!ctx.active)
		return ;
	kill(ctx.pid, SIGKILL);
	waitpid(ctx.pid, NULL, 0);
	if (ctx.inFd != -1)
		remove_PipeFd(fds, ctx.inFd, true);
	if (ctx.outFd != -1)
		remove_PipeFd(fds, ctx.outFd, true);
	ctx.reset();
}


void Server::process_CgiWrite(std::vector<pollfd> &fds, size_t i)
{
    int         pipeFd   = fds[i].fd;
    int         clientFd = _pipeToClient[pipeFd];
    Client     &client   = _allClients[clientFd];
    CgiContext &ctx      = client.cgi;
    const char *data    = ctx.bodyToSend.c_str() + ctx.bodyOffset;
    size_t      rem     = ctx.bodyToSend.size() - ctx.bodyOffset;
    ssize_t written = write(pipeFd, data, rem);
    if (written > 0)
    {
        ctx.bodyOffset += (size_t)written;
        if (ctx.bodyOffset >= ctx.bodyToSend.size())
        {
            remove_PipeFd(fds, pipeFd, true);
            ctx.inFd = -1;
        }
        return;
    }
    if (written == -1)
        return;
    remove_PipeFd(fds, pipeFd, true);
    ctx.inFd = -1;
}


void Server::process_CgiRead(std::vector<pollfd> &fds, size_t i)
{
	int		pipeFd;
	int		clientFd;
	char	buf[4096];
	ssize_t	n;
	int		waitStatus;

	pipeFd = fds[i].fd;
	clientFd = _pipeToClient[pipeFd];
	Client &client = _allClients[clientFd];
	CgiContext &ctx = client.cgi;
	n = read(pipeFd, buf, sizeof(buf));
	if (n > 0)
	{
		if (ctx.output.size() + (size_t)n > MAX_CGI_OUTPUT)
		{
			abort_Cgi(client, fds);
			client.response = _router.make_ErrorCode(502);
			std::string raw = client.response.serialize();
			client.writeBuffer.write(raw.c_str(), raw.size());
			for (size_t j = 0; j < fds.size(); j++)
			{
				if (fds[j].fd == clientFd)
				{
					fds[j].events |= POLLOUT;
					break ;
				}
			}
			return ;
		}
		ctx.output.append(buf, n);
		return ;
	}
	if (n == -1)
	{
		if (errno == EAGAIN || errno == EINTR)
			return ; // No data right now — poll() will fire again
		remove_PipeFd(fds, pipeFd, true);
		ctx.outFd = -1;
		if (ctx.inFd != -1)
		{
			remove_PipeFd(fds, ctx.inFd, true);
			ctx.inFd = -1;
		}
		kill(ctx.pid, SIGKILL);
		waitpid(ctx.pid, NULL, 0);
		ctx.reset();
		client.response = _router.make_ErrorCode(502);
		std::string raw = client.response.serialize();
		client.writeBuffer.write(raw.c_str(), raw.size());
		for (size_t j = 0; j < fds.size(); j++)
		{
			if (fds[j].fd == clientFd)
			{
				fds[j].events |= POLLOUT;
				break ;
			}
		}
		return ;
	}
	// n == 0: real EOF — CGI process closed stdout
	remove_PipeFd(fds, pipeFd, true);
	ctx.outFd = -1;
		if (ctx.inFd != -1)
	{
		remove_PipeFd(fds, ctx.inFd, true);
		ctx.inFd = -1;
	}
	waitStatus = 0;
	waitpid(ctx.pid, &waitStatus, 0);
	client.response = _router.cgi->finish(ctx, waitStatus);
	ctx.reset();
	std::string raw = client.response.serialize();
	client.writeBuffer.write(raw.c_str(), raw.size());
	for (size_t j = 0; j < fds.size(); j++)
	{
		if (fds[j].fd == clientFd)
		{
			fds[j].events |= POLLOUT;
			break ;
		}
	}
}

bool Server::dispatch_CgiWrite(std::vector<Server> &servers,
	std::vector<pollfd> &fds, size_t i)
{
	int	pipeFd;

	pipeFd = fds[i].fd;
	for (size_t s = 0; s < servers.size(); s++)
	{
		if (servers[s]._pipeToClient.count(pipeFd))
		{
			servers[s].process_CgiWrite(fds, i);
			return true;
		}
	}
	return false;
}

bool Server::dispatch_CgiRead(std::vector<Server> &servers,
	std::vector<pollfd> &fds, size_t i)
{
	int	pipeFd;

	pipeFd = fds[i].fd;
	for (size_t s = 0; s < servers.size(); s++)
	{
		if (servers[s]._pipeToClient.count(pipeFd))
		{
			servers[s].process_CgiRead(fds, i);
			return true;
		}
	}
	return false;
}