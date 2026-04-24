/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/03/24 02:59:09 by rafael           ###   ########.fr       */
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
#include <signal.h>

enum	SendStatus
{
	SEND_OK,
	SEND_DONE,
	SEND_ERROR
};

// static allows me to call a function without object
class Server
{
  private:
	int _server_fd;
	int _port;
	std::map<int, Client> _allClients; // store each fd to each client
	Router _router;

  public:
	Server(int port, ServerConfig &sc);
	~Server();
	sockaddr_in create_Address();
	int setup_Socket();
	int accept_NewClient(std::vector<pollfd> &fds);
	bool receive_FromClient(std::vector<pollfd> &fds, size_t index);
	SendStatus send_ToClient(std::vector<pollfd> &fds, size_t index);
	void cleanup_TimeoutClients(std::vector<pollfd> &fds, time_t now, int timeoutSec);
    static void build_PollList(std::vector<Server> &servers, std::vector<pollfd> &fds);
    static bool try_AcceptClient(std::vector<Server> &servers, std::vector<pollfd> &fds, int fd);
    static bool process_ClientRead(std::vector<Server> &servers, std::vector<pollfd> &fds, size_t i);
    static bool process_ClientWrite(std::vector<Server> &servers, std::vector<pollfd> &fds, size_t i);
    static void close_AllClients(std::vector<Server> &servers);
	static void handle_Clients(std::vector<Server> &servers);
};
void handle_Sigint(int sig);

#endif
