#include <server/server.hpp>
#include <http/Request.hpp>
#include <http/Router.hpp>


Server::Server(int port) : _port(port)
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
sockaddr_in Server::create_address()
{
	sockaddr_in	addr;

	ft_memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// Accept connections from outside computers and from this computer itself (localhost)
	return (addr);
}

int Server::setup_socket()
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
	// allow reusing the port immediately after the socket is closed
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
	addr = create_address();
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

int Server::accept_new_client(std::vector<pollfd> &fds)
{
	pollfd		poll;
	sockaddr_in	client_addr;
	socklen_t	client_len;
	int			client_fd;

	client_len = sizeof(client_addr);
	;
	client_fd = accept(_server_fd, (struct sockaddr *)&client_addr,
			&client_len);
	if (client_fd == -1)
	{
		std::cerr << "Error: accept failed\n";
		return (-1);
	}
	std::cout << "Client connected: fd=" << client_fd << "\n";
	int flags = fcntl(client_fd, F_GETFL, 0); // F_GETFL get the current file status flags,like O_NONBLOCK or O_RDONLY
	fcntl(client_fd, F_SETFL, flags | O_NONBLOCK); // F_SETFL sets the flags we retrieved above
	poll.fd = client_fd;
	poll.events = POLLIN;
	fds.push_back(poll);
	_allClients[client_fd] = Client(client_fd);
	return (client_fd);
}

bool Server::receive_from_client(std::vector<pollfd> &fds, size_t index)
{
	int		client_fd;
	int		bytes_received;
	char	buffer[1024];
	char	temp[1024];
	size_t	n;
	Router router;

	client_fd = fds[index].fd;
	Client &client = _allClients[client_fd];
	bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received > 0)
	{
		buffer[bytes_received] = '\0';
		client.readBuffer.write(buffer, bytes_received);
		std::cout << "Client " << client_fd << ": " << buffer << "\n";
		n = client.readBuffer.read(temp, sizeof(temp));
		std::string chunk(temp, n);
		client.request.fillBuffer(chunk, chunk.size());
		std::cout << "BEFORE DONE" << std::endl;
 		if (client.request.isDone())
		{
			std::cout << "INSIDE DONE" << std::endl;
			client.response = router.handleRequest(client.request);
			std::string rawResponse = client.response.serialize();
			std::cout << "=== RAW RESPONSE BEGIN ===\n";
			std::cout << rawResponse << "\n";
			std::cout << "=== RAW RESPONSE END ===\n";
			client.writeBuffer.write(rawResponse.c_str(), rawResponse.size());
			fds[index].events |= POLLOUT; //Enables POLLOUT on this socket so the server knows it must send data
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

SendStatus Server::send_to_client(std::vector<pollfd> &fds, size_t index)
{
	int fd = fds[index].fd;
	Client &client = _allClients[fd];
	size_t available = client.writeBuffer.getSize();
	if(available == 0)
		return SEND_DONE;
	char temp[1024];
	size_t toSend = std::min(available, sizeof(temp));
	size_t copied = client.writeBuffer.peek(temp, toSend);
	size_t sent = send(fd, temp, copied, 0);
	if(sent <= 0)
		return SEND_ERROR;
	client.writeBuffer.consume(sent);
	if(client.writeBuffer.getSize() == 0)
		return SEND_DONE;
	return SEND_OK;
}

void Server::handle_clients()
{
	int		ret;
	pollfd	listen_fd;
	std::vector<pollfd> fds;
	listen_fd.fd = _server_fd;
	listen_fd.events = POLLIN; // the listening socket only cares about POLLIN
	fds.push_back(listen_fd);
	while (true)
	{
		ret = poll(fds.data(), fds.size(), -1);
		if (ret == -1)
		{
			std::cerr << "Error: poll failed\n";
			continue ;
		}
		for (size_t i = 0; i < fds.size(); ++i)
		{
			// if the POLLIN bit is inside revents (int containing bit flags)
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == _server_fd)
					accept_new_client(fds);
				else
					receive_from_client(fds, i);
			}
			if (fds[i].revents & POLLOUT)
			{
				SendStatus status = send_to_client(fds, i);
				if (status == SEND_DONE || status == SEND_ERROR)
				{
					close(fds[i].fd);
					_allClients.erase(fds[i].fd);
					fds.erase(fds.begin() + i);
					break;
				}
			}
		}
	}
}
