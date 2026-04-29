/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/27 03:49:39 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <arpa/inet.h>
# include <core/client.hpp>
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <http/routing/Router.hpp>
# include <iostream>
# include <map>
# include <netdb.h>
# include <netinet/in.h>
# include <poll.h>
# include <signal.h>
# include <string.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>
# include <vector>

enum	SendStatus
{
	SEND_OK,
	SEND_DONE,
	SEND_ERROR
};

class Server
{
  private:
	int _server_fd;
	int _port;
	std::map<int, Client> _allClients; // client_fd  → Client
	std::map<int, int> _pipeToClient;  // pipe_fd    → client_fd
	Router _router;

	bool start_Cgi(Client &client, const Request &req,
		std::vector<pollfd> &fds, unsigned long tick);
	void process_CgiWrite(std::vector<pollfd> &fds, size_t i);
	void process_CgiRead(std::vector<pollfd> &fds, size_t i);
	void remove_PipeFd(std::vector<pollfd> &fds, int fd, bool doClose);
	void abort_Cgi(Client &client, std::vector<pollfd> &fds);

  public:
	Server(int port, ServerConfig &sc);
	~Server();

	sockaddr_in create_Address();
	int setup_Socket();
	int accept_NewClient(std::vector<pollfd> &fds, unsigned long tick);
	bool receive_FromClient(std::vector<pollfd> &fds, size_t index, unsigned long tick);
	SendStatus send_ToClient(std::vector<pollfd> &fds, size_t index);
	void cleanup_TimeoutClients(std::vector<pollfd> &fds, unsigned long tick, int timeoutTicks);

	static void build_PollList(std::vector<Server> &servers,
		std::vector<pollfd> &fds);
	static bool try_AcceptClient(std::vector<Server> &servers,
		std::vector<pollfd> &fds, int fd, unsigned long tick);
	static bool process_ClientRead(std::vector<Server> &servers,
		std::vector<pollfd> &fds, size_t i, unsigned long tick);
	static bool process_ClientWrite(std::vector<Server> &servers,
		std::vector<pollfd> &fds, size_t i);

	static bool dispatch_CgiWrite(std::vector<Server> &servers,
		std::vector<pollfd> &fds, size_t i);
	static bool dispatch_CgiRead(std::vector<Server> &servers,
		std::vector<pollfd> &fds, size_t i);

	static void close_AllClients(std::vector<Server> &servers);
	static void handle_Clients(std::vector<Server> &servers);
};
void	add_PollFd(std::vector<pollfd> &fds, int fd, short events);
void	handle_Sigint(int sig);

#endif
