#include "../includes/server/server.hpp"

server::server(int port) : _port(port)
{
}

server::~server()
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
sockaddr_in server::create_address()
{
	sockaddr_in	addr;

	ft_memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
		// Accept connections from outside computers and from this computer itself (localhost)
	return (addr);
}

int server::setup_socket()
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

int server::accept_new_client(std::vector<pollfd> &fds)
{
	int			flags;
	int			client_fd;
	sockaddr_in	client_addr;
	socklen_t	client_len;
	pollfd		poll_fd;

	client_len = sizeof(client_addr);
	client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd == -1)
	{
		std::cerr << "Error: accept failed\n";
		return (-1);
	}
	std::cout << "Client connected: fd=" << client_fd << "\n";
	flags = fcntl(client_fd, F_GETFL, 0); // F_GETFL get the current file status flags, like O_NONBLOCK or O_RDONLY
	fcntl(client_fd, F_SETFL, flags | O_NONBLOCK); // F_SETFL sets the flags we retrieved above 
	poll_fd.fd = client_fd;
	poll_fd.events = POLLIN;
	fds.push_back(poll_fd);
	return (client_fd);
}

bool server::handle_client_data(std::vector<pollfd> &fds, size_t index)
{
	int		client_fd;
	char	buffer[1024];
	int		bytes_received;

	client_fd = fds[index].fd;
	bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received > 0)
	{
		buffer[bytes_received] = '\0';
		std::cout << "Client " << client_fd << ": " << buffer << "\n";
		return (true);
	}
	else
	{
		if (bytes_received == 0)
			std::cout << "Client disconnected: fd=" << client_fd << "\n";
		else
			std::cerr << "Error receiving from client fd=" << client_fd << "\n";
		close(client_fd);
		fds.erase(fds.begin() + index);
		return (false);
	}
}

void server::accept_clients()
{
	pollfd	listen_fd;
	int		ret;

	std::vector<pollfd> fds;
	listen_fd.fd = _server_fd;
	listen_fd.events = POLLIN;
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
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == _server_fd)
					accept_new_client(fds);
				else
					handle_client_data(fds, i);
			}
		}
	}
}
