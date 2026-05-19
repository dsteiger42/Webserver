/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/05/19 04:38:45 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <core/server.hpp>
#include <errno.h>
#include <http/cgi/CGI.hpp>
#include <http/request/Request.hpp>
#include <http/routing/Router.hpp>
#include <signal.h>
#include <utils/signals/signals.hpp>

static void	*ft_memset(void *str, int c, size_t n)
{
	unsigned char	*mem;

	mem = (unsigned char *)str;
	for (size_t i = 0; i < n; i++)
		mem[i] = (unsigned char)c;
	return (str);
}

void	add_PollFd(std::vector<pollfd> &fds, int fd, short events)
{
	pollfd	pfd;

	ft_memset(&pfd, 0, sizeof(pfd));
	pfd.fd = fd;
	pfd.events = events;
	fds.push_back(pfd);
}

Server::Server(int port, ServerConfig &sc) : _server_fd(-1), _port(port),
	_router(sc)
{
}

Server::~Server()
{
	if (_server_fd != -1)
		close(_server_fd);
}

sockaddr_in Server::create_Address()
{
	sockaddr_in	addr;

	ft_memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	return (addr);
}

int Server::setup_Socket()
{
	int			opt;
	sockaddr_in	addr;

	opt = 1;
	_server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_fd == -1)
	{
		std::cerr << "Error: failed to create socket\n";
		return (-1);
	}
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
		-1)
	{
		std::cerr << "Error: setsockopt failed\n";
		return (-1);
	}
	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: fcntl O_NONBLOCK on server socket\n";
		return (-1);
	}
	addr = create_Address();
	if (bind(_server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		std::cerr << "Error: failed to bind socket\n";
		return (-1);
	}
	if (listen(_server_fd, SOMAXCONN) == -1)
	{
		std::cerr << "Error: failed to listen\n";
		return (-1);
	}
	return (0);
}

int Server::accept_NewClient(std::vector<pollfd> &fds, unsigned long tick)
{
	pollfd		poll;
	sockaddr_in	client_addr;
	socklen_t	client_len;
	int			client_fd;
	size_t		maxBodySize;

	client_len = sizeof(client_addr);
	client_fd = accept(_server_fd, (struct sockaddr *)&client_addr,
			&client_len);
	if (client_fd == -1)
	{
		std::cerr << "Error: accept failed\n";
		return (-1);
	}
	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: fcntl O_NONBLOCK failed on client fd=" << client_fd << "\n";
		close(client_fd);
		return (-1);
	}
	ft_memset(&poll, 0, sizeof(poll));
	poll.fd = client_fd;
	poll.events = POLLIN;
	fds.push_back(poll);
	maxBodySize = _router.get_Config().config.client_max_body_size;
	_allClients[client_fd] = Client(client_fd, tick, maxBodySize);
	std::cout << "\033[32m[Client " << _allClients[client_fd].id << " (FD " << client_fd << ")] Connected\033[0m\n";
	_allClients[client_fd].request.set_MaxBodySize(maxBodySize);
	return (client_fd);
}

bool Server::receive_FromClient(std::vector<pollfd> &fds, size_t index,
	unsigned long tick)
{
	int		client_fd;
	char	buffer[1024];
	int		bytes_received;

	client_fd = fds[index].fd;
	Client &client = _allClients[client_fd];

	bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received > 0)
	{
		client.lastActivity = tick;

		if (client.drain)
			return (true);

		std::string chunk(buffer, bytes_received);
		client.request.fill_Buffer(chunk, chunk.size());

		while (client.request.is_Done() || (!client.request.get_validRequest()
				&& client.request.get_statusCode() != 0))
		{
			if (!client.request.get_validRequest())
				std::cout << "\033[31m[Client " << client.id << " (FD "
					<< client_fd << ")] Invalid Request (Status "
					<< client.request.get_statusCode() << ")\033[0m\n";
			else
				std::cout << "\033[34m[Client " << client.id << " (FD "
					<< client_fd << ")] Request: " << client.request.get_Method()
					<< " " << client.request.get_Path() << "\033[0m\n";

			if (_router.is_CgiRequest(client.request))
			{
				std::cout << "\033[33m[Client " << client.id << " (FD "
					<< client_fd << ")] Launching CGI...\033[0m\n";
				if (!start_Cgi(client, client.request, fds, tick))
					fds[index].events |= POLLOUT;
				client.request.reset();
				break ;
			}

			client.response = _router.handle_Request(client.request);
			std::cout << "\033[36m[Client " << client.id << " (FD "
				<< client_fd << ")] Response: "
				<< client.response.get_StatusCode() << "\033[0m\n";

			std::string raw = client.response.serialize();
			client.writeBuffer.write(raw.c_str(), raw.size());
			fds[index].events |= POLLOUT;

			if (client.response.get_StatusCode() >= 400)
			{
				client.shouldClose = true;
				client.drain = true;
			}

			const std::map<std::string, std::string>& req_headers =
				client.request.get_Headers();
			std::map<std::string, std::string>::const_iterator conn_it =
				req_headers.find("connection");
			if (conn_it != req_headers.end())
			{
				std::string conn_val = conn_it->second;
				for (size_t i = 0; i < conn_val.length(); i++)
					if (conn_val[i] >= 'A' && conn_val[i] <= 'Z')
						conn_val[i] += 32;
				if (conn_val == "keep-alive")
					client.shouldClose = false;
				else
					client.shouldClose = true;
			}
			else
				client.shouldClose = true;

			std::string leftover = client.request.get_Leftover();
			client.request.reset();
			if (leftover.empty())
				break ;
			client.request.fill_Buffer(leftover, leftover.size());
		}
		return (true);
	}
	else
	{
		if (bytes_received == 0)
			std::cout << "\033[35m[Client " << client.id << " (FD "
				<< client_fd << ")] Disconnected\033[0m\n";
		else
			std::cerr << "Error receiving from client fd=" << client_fd << "\n";
		if (client.cgi.active)
			abort_Cgi(client, fds);
		close(client_fd);
		_allClients.erase(client_fd);
		fds.erase(fds.begin() + index);
		return (false);
	}
}
SendStatus Server::send_ToClient(std::vector<pollfd> &fds, size_t index)
{
	int		fd;
	int		sent;
	size_t	available;
	char	temp[65536];
	size_t	toSend;
	size_t	copied;

	fd = fds[index].fd;
	Client &client = _allClients[fd];
	available = client.writeBuffer.get_Size();
	if (available == 0)
		return (SEND_DONE);
	toSend = std::min(available, sizeof(temp));
	copied = client.writeBuffer.peek(temp, toSend);
	sent = send(fd, temp, copied, 0);
	if (sent < 0)
		return (SEND_OK);
	if (sent == 0)
		return (SEND_ERROR);
	client.writeBuffer.consume(sent);
	if (client.writeBuffer.get_Size() == 0)
		return (SEND_DONE);
	return (SEND_OK);
}

void Server::cleanup_TimeoutClients(std::vector<pollfd> &fds,
	unsigned long tick, int timeoutTicks)
{
	const int	INCOMPLETE_REQUEST_TIMEOUT_TICKS = 30;
	bool		doKill;
	int			fd;

	std::map<int, Client>::iterator it = _allClients.begin();
	while (it != _allClients.end())
	{
		fd = it->first;
		Client &client = it->second;
		doKill = false;

		if (client.cgi.active && (tick - client.cgi.startTime) > 100)
		{
			std::cout << "\033[35m[Timeout] CGI timeout for client "
				<< fd << "\033[0m\n";
			abort_Cgi(client, fds);
			client.response = _router.make_ErrorCode(504);
			std::string raw = client.response.serialize();
			client.writeBuffer.write(raw.c_str(), raw.size());
			client.shouldClose = true;
			client.drain = true;
			for (size_t i = 0; i < fds.size(); i++)
			{
				if (fds[i].fd == fd)
				{
					fds[i].events = POLLOUT;
					break ;
				}
			}
			++it;
			continue ;
		}
		if (client.drain || client.writeBuffer.get_Size() > 0)
		{
			if (client.writeBuffer.get_Size() == 0
				&& (tick - client.lastActivity) > (unsigned long)timeoutTicks)
				doKill = true;
		}
		else if (!client.request.is_Done() && !client.cgi.active)
		{
			if (tick - client.lastActivity
				> (unsigned long)INCOMPLETE_REQUEST_TIMEOUT_TICKS)
			{
				std::cout << "\033[31m[Timeout] Client " << fd
					<< " timed out (incomplete request)\033[0m\n";
				client.response = _router.make_ErrorCode(408);
				std::string raw = client.response.serialize();
				client.writeBuffer.write(raw.c_str(), raw.size());
				client.shouldClose = true;
				client.drain = true;
				for (size_t i = 0; i < fds.size(); i++)
				{
					if (fds[i].fd == fd)
					{
						fds[i].events = POLLOUT;
						break ;
					}
				}
				++it;
				continue ;
			}
		}
		else if (!client.cgi.active
			&& (tick - client.lastActivity) > (unsigned long)timeoutTicks)
			doKill = true;
		if (doKill)
		{
			close(fd);
			for (size_t i = 0; i < fds.size(); i++)
			{
				if (fds[i].fd == fd)
				{
					fds.erase(fds.begin() + i);
					break ;
				}
			}
			std::map<int, Client>::iterator toErase = it;
			++it;
			_allClients.erase(toErase);
		}
		else
			++it;
	}
}

void Server::build_PollList(std::vector<Server> &servers,
	std::vector<pollfd> &fds)
{
	for (size_t i = 0; i < servers.size(); i++)
		add_PollFd(fds, servers[i]._server_fd, POLLIN);
}

bool Server::try_AcceptClient(std::vector<Server> &servers,
	std::vector<pollfd> &fds, int fd, unsigned long tick)
{
	for (size_t s = 0; s < servers.size(); s++)
	{
		if (fd == servers[s]._server_fd)
		{
			servers[s].accept_NewClient(fds, tick);
			return (true);
		}
	}
	return (false);
}

bool Server::process_ClientRead(std::vector<Server> &servers,
	std::vector<pollfd> &fds, size_t i, unsigned long tick)
{
	for (size_t s = 0; s < servers.size(); s++)
	{
		if (servers[s]._allClients.count(fds[i].fd))
			return (servers[s].receive_FromClient(fds, i, tick));
	}
	return (true);
}

bool Server::process_ClientWrite(std::vector<Server> &servers,
	std::vector<pollfd> &fds, size_t i)
{
	int			fd;
	SendStatus	status;
	int			code;

	for (size_t s = 0; s < servers.size(); s++)
	{
		if (!servers[s]._allClients.count(fds[i].fd))
			continue ;

		fd = fds[i].fd;
		Client &client = servers[s]._allClients[fd];
		status = servers[s].send_ToClient(fds, i);

		if (status == SEND_OK)
			return (true);

		if (status == SEND_DONE)
		{
			code = client.response.get_StatusCode();

			if (code == 413 || code == 431)
			{
				client.drain = true;
				client.shouldClose = true;
				fds[i].events = POLLIN;
				return (true);
			}

			close(fd);
			servers[s]._allClients.erase(fd);
			fds.erase(fds.begin() + i);
			return (false);
		}

		close(fd);
		servers[s]._allClients.erase(fd);
		fds.erase(fds.begin() + i);
		return (false);
	}
	return (true);
}

void Server::close_AllClients(std::vector<Server> &servers)
{
	for (size_t s = 0; s < servers.size(); s++)
	{
		for (std::map<int,
			Client>::iterator it = servers[s]._allClients.begin(); it != servers[s]._allClients.end(); ++it)
			close(it->first);
		servers[s]._allClients.clear();
		servers[s]._pipeToClient.clear();
	}
}

void Server::handle_Clients(std::vector<Server> &servers)
{
	const int		POLL_TIMEOUT_MS = 325;
	const int		CLIENT_TIMEOUT_TICKS = 15;
	unsigned long	tick;
	int				ret;
	short			revents;
	int				fd;
	bool			isCgiPipe;

	tick = 0;
	std::vector<pollfd> fds;
	build_PollList(servers, fds);
	while (g_running)
	{
		ret = poll(fds.data(), fds.size(), POLL_TIMEOUT_MS);
		if (ret == -1)
		{
			if (errno == EINTR)
				continue ;
			std::cerr << "Error: poll failed\n";
			break ;
		}
		++tick;
		for (size_t i = 0; i < fds.size();)
		{
			revents = fds[i].revents;
			fd = fds[i].fd;
			if (revents == 0)
			{
				++i;
				continue ;
			}
			if ((revents & POLLIN) && try_AcceptClient(servers, fds, fd, tick))
			{
				++i;
				continue ;
			}
			isCgiPipe = false;
			for (size_t s = 0; s < servers.size(); s++)
			{
				if (servers[s]._pipeToClient.count(fd))
				{
					isCgiPipe = true;
					break ;
				}
			}
			if (isCgiPipe)
			{
				if (revents & POLLOUT)
					dispatch_CgiWrite(servers, fds, i, tick);
				if (i < fds.size() && fds[i].fd == fd && (revents & (POLLIN | POLLHUP)))
					dispatch_CgiRead(servers, fds, i, tick);
				if (i < fds.size() && fds[i].fd == fd)
					++i;
				continue ;
			}
			if (revents & POLLIN)
			{
				if (!process_ClientRead(servers, fds, i, tick))
					continue ;
			}
			if (i < fds.size() && fds[i].fd == fd && (revents & POLLOUT))
			{
				if (!process_ClientWrite(servers, fds, i))
					continue ;
			}
			++i;
		}
		for (size_t s = 0; s < servers.size(); s++)
			servers[s].cleanup_TimeoutClients(fds, tick, CLIENT_TIMEOUT_TICKS);
	}
	close_AllClients(servers);
}
