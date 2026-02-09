#include "server.hpp"

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
port is 16 bit value. IP address is 32 bit value. 
For 16 bit value, use htons or ntohs
For 32 bit value, use htonl or ntohl
Internet protocols work on Big-Endian (network order) -> most significant byte is stored first
Most computers work in Small-Endian -> least significant byte is stored first
I need to translate the Port and Ip Address to network order, hence the htons(_port).
And, if needed, IP address will be used with htonl.
*/
// sockaddr_in represents the adress of a socket
sockaddr_in server::create_address()
{
	sockaddr_in	addr;

	ft_memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from outside computers and from this computer itself (localhost)
	return (addr);
}

int server::setup_socket()
{
	sockaddr_in	addr;

	_server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_fd == -1)
	{
		std::cerr << "Error: failed to create socket" << std::endl;
		return (-1);
	}
	// allow reusing the port immediately after the socket is closed
	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
		-1)
	{
		std::cerr << "Error: setsockopt failed" << std::endl;
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

void server::accept_clients()
{
	sockaddr_in	client_addr;
	socklen_t	client_len = sizeof(client_addr);
	// might change to a vector for dynamic allocation, if I want to use "client_body_buffer_size 10000;" instead
	char		buffer[1024];
	int			client_fd;
	int			bytes_received;

	while (true)
	{
		client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &client_len);
		if (client_fd == -1)
		{
			std::cerr << "Error: failed to accept client" << std::endl;
			continue ;
		}
		std::cout << "Client connected" << std::endl;
		while ((bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0)
		{
			buffer[bytes_received] = '\0';
			std::cout << buffer << std::endl;
		}
		if (bytes_received == 0)
			std::cout << "Client disconnected" << std::endl;
		else if (bytes_received == -1)
			std::cerr << "Error: problem receiving data" << std::endl;
		close(client_fd);
	}
}
