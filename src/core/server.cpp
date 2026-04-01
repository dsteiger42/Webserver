/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/01 16:17:14 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <core/server.hpp>
#include <http/request/Request.hpp>
#include <http/routing/Router.hpp>
#include <utils/signals/signals.hpp>

Server::Server(int port, ServerConfig &sc) : _port(port), _router(sc)
{
}

Server::~Server()
{
}

void	*ft_memset(void *str, int c, size_t n)
{
	unsigned char	*mem;
	size_t			i;

	mem = (unsigned char *)str;
	i = 0;
	while (i < n)
		mem[i++] = c;
	return (mem);
}

/*
Port is 16 bit value. IP address is 32 bit value.
For 16 bit value, use htons or ntohs
For 32 bit value, use htonl or ntohl
Internet protocols work on Big-Endian (network order)
	-> most significant byte is stored first
Most computers work in Small-Endian -> least significant byte is stored first
I need to translate the Port and Ip Address to network order,
	hence the htons(_port).
And, if needed, IP address will be used with htonl.
*/
// sockaddr_in represents the adress of a socket
sockaddr_in Server::create_Address()
{
	sockaddr_in	addr;

	ft_memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from outside computers and from this computer itself (localhost)
	return (addr);
}

int Server::setup_Socket()
{
	sockaddr_in	addr;
	int			opt;
	int			flags;

	_server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_fd == -1)
	{
		std::cerr << "Error: failed to create socket" << std::endl;
		return (-1);
	}
	opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
		-1)
	{
		std::cerr << "Error: setsockopt failed" << std::endl;
		return (-1);
	}
	flags = fcntl(_server_fd, F_GETFL, 0);
	if (flags == -1)
	{
		std::cerr << "Error: fcntl F_GETFL" << std::endl;
		return (-1);
	}
	if (fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "Error: fcntl F_SETFL O_NONBLOCK" << std::endl;
		return (-1);
	}
	addr = create_Address();
	if (bind(_server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		std::cerr << "Error: failed to bind socket" << std::endl;
		return (-1);
	}
	if (listen(_server_fd, SOMAXCONN) == -1)
	{
		std::cerr << "Error: failed to listen socket" << std::endl;
		return (-1);
	}
	return (0);
}

int Server::accept_NewClient(std::vector<pollfd> &fds)
{
	pollfd		poll;
	sockaddr_in	client_addr;
	socklen_t	client_len;
	int			client_fd;
	int			flags;

	client_len = sizeof(client_addr);
	client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd == -1)
	{
		std::cerr << "Error: accept failed\n";
		return (-1);
	}
	std::cout << "Client connected: fd=" << client_fd << "\n";
	flags = fcntl(client_fd, F_GETFL, 0); // F_GETFL get the current file status flags,like O_NONBLOCK or O_RDONLY
	fcntl(client_fd, F_SETFL, flags | O_NONBLOCK); // F_SETFL sets the flags we retrieved above
	ft_memset(&poll, 0, sizeof(poll));
	poll.fd = client_fd;
	poll.events = POLLIN;
	fds.push_back(poll);
	_allClients[client_fd] = Client(client_fd);
	_allClients[client_fd].lastActivity = time(NULL);
	_allClients[client_fd].requestStart = time(NULL);
	return (client_fd);
}

bool Server::receive_FromClient(std::vector<pollfd> &fds, size_t index)
{
	int		client_fd;
	int		bytes_received;
	char	buffer[1024];
	char	temp[1024];
	size_t	n;

	client_fd = fds[index].fd;
	Client &client = _allClients[client_fd];
	bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received > 0)
	{
		buffer[bytes_received] = '\0';
		if (client.readBuffer.get_Size() == 0)
			client.requestStart = time(NULL);
		client.lastActivity = time(NULL);
		client.readBuffer.write(buffer, bytes_received);
		std::cout << "Client " << client_fd << ": " << buffer << "\n";
		n = client.readBuffer.read(temp, sizeof(temp));
		std::string chunk(temp, n);
		client.request.fill_Buffer(chunk, chunk.size());
		if (client.request.is_Done()|| !client.request.get_validRequest())
		{
			client.response = _router.handle_Request(client.request);
			std::string rawResponse = client.response.serialize();
			client.writeBuffer.write(rawResponse.c_str(), rawResponse.size());
			fds[index].events |= POLLOUT;
			client.request.reset();
		}
		return (true);
	}
	else
	{
		if (bytes_received == 0)
			std::cout << "Client disconnected: fd=" << client_fd << "\n";
		else
			std::cerr << "Error receiving from client fd=" << client_fd << "\n";
		close(client_fd);
		_allClients.erase(client_fd);
		fds.erase(fds.begin() + index);
		return (false);
	}
}

SendStatus Server::send_ToClient(std::vector<pollfd> &fds, size_t index)
{
	int		fd;
	size_t	available;
	char	temp[1024];
	size_t	toSend;
	size_t	copied;
	size_t	sent;

	fd = fds[index].fd;
	Client &client = _allClients[fd];
	available = client.writeBuffer.get_Size();
	if (available == 0)
		return (SEND_DONE);
	toSend = std::min(available, sizeof(temp));
	copied = client.writeBuffer.peek(temp, toSend);
	sent = send(fd, temp, copied, 0);
	if (sent <= 0)
		return (SEND_ERROR);
	client.writeBuffer.consume(sent);
	if (client.writeBuffer.get_Size() == 0)
		return (SEND_DONE);
	return (SEND_OK);
}

void Server::cleanup_TimeoutClients(std::vector<pollfd> &fds, time_t now,
	int timeoutSec)
{
	int		fd;
	bool	timeout;

	std::map<int, Client>::iterator it = _allClients.begin();
	while (it != _allClients.end())
	{
		fd = it->first;
		Client &client = it->second;
		timeout = false;
		if (!client.request.is_Done())
		{
			if (now - client.requestStart > timeoutSec)
				timeout = true;
		}
		else
		{
			if (now - client.lastActivity > timeoutSec)
				timeout = true;
		}
		if (timeout)
		{
			std::cout << "Client timed out: fd=" << fd << "\n";
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

void Server::handle_Clients(std::vector<Server> &servers)
{
	int			ret;
	bool		is_server_fd;
	pollfd		listen_fd;
	SendStatus	status;
	time_t		now;

	const int POLL_TIMEOUT_MS = 1000;  // up every 1 second
	const int CLIENT_TIMEOUT_SEC = 10; // Max request time
	std::vector<pollfd> fds; // Register all server listening fds
	for (size_t s = 0; s < servers.size(); s++)
	{
		listen_fd.fd = servers[s]._server_fd;
		listen_fd.events = POLLIN;
		fds.push_back(listen_fd);
	}
	while (g_running)
	{
		ret = poll(fds.data(), fds.size(), POLL_TIMEOUT_MS);
		if (ret == -1)
		{
			if (errno == EINTR)
				 continue;
			std::cerr << "Error: poll failed\n";
			break;
		}
		now = time(NULL);
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				is_server_fd = false;
				for (size_t s = 0; s < servers.size(); s++)
				{
					if (fds[i].fd == servers[s]._server_fd)
					{
						servers[s].accept_NewClient(fds);
						is_server_fd = true;
						break ;
					}
				}
				if (!is_server_fd)
				{
					for (size_t s = 0; s < servers.size(); s++)
					{
						if (servers[s]._allClients.count(fds[i].fd))
						{
							servers[s].receive_FromClient(fds, i);
							break ;
						}
					}
				}
			}
			if (fds[i].revents & POLLOUT)
			{
				for (size_t s = 0; s < servers.size(); s++)
				{
					if (servers[s]._allClients.count(fds[i].fd))
					{
						status = servers[s].send_ToClient(fds, i);
						if (status == SEND_DONE || status == SEND_ERROR)
						{
							close(fds[i].fd);
							servers[s]._allClients.erase(fds[i].fd);
							fds.erase(fds.begin() + i);
							break ;
						}
						break ;
					}
				}
			}
		}
		for (size_t s = 0; s < servers.size(); s++)
			servers[s].cleanup_TimeoutClients(fds, now, CLIENT_TIMEOUT_SEC);
	}
}
