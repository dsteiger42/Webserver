/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/25 04:22:39 by rafael           ###   ########.fr       */
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

enum SendStatus
{
    SEND_OK,
    SEND_DONE,
    SEND_ERROR
};

class Server
{
  private:
    int                  _server_fd;
    int                  _port;
    std::map<int, Client> _allClients;   // client_fd  → Client
    std::map<int, int>   _pipeToClient;  // pipe_fd    → client_fd
    Router               _router;

    // ----------------------------------------------------------------
    // CGI async helpers (instance methods — need access to _allClients
    // and _pipeToClient)
    // ----------------------------------------------------------------

    /*
    ** Called from process_ClientRead when the router decides CGI is needed.
    ** Calls CGI::launch(), registers the pipe fds in fds[], and updates
    ** _pipeToClient.  Returns false if launch fails (caller sends 500).
    */
    bool start_Cgi(Client &client, const Request &req,
                   std::vector<pollfd> &fds);

    /*
    ** Called from handle_Clients when POLLOUT fires on a CGI inFd.
    ** Writes the next chunk of bodyToSend into the pipe (non-blocking).
    ** When all bytes are written, closes inFd and removes it from fds[].
    */
    void process_CgiWrite(std::vector<pollfd> &fds, size_t i);

    /*
    ** Called from handle_Clients when POLLIN fires on a CGI outFd.
    ** Reads available bytes into ctx.output.
    ** On EOF: closes outFd, calls waitpid, calls CGI::finish(),
    ** serialises the response into client.writeBuffer, removes the fd.
    */
    void process_CgiRead(std::vector<pollfd> &fds, size_t i);

    /*
    ** Removes a pipe fd from fds[] and from _pipeToClient.
    ** Optionally closes the fd.
    */
    void remove_PipeFd(std::vector<pollfd> &fds, int fd, bool doClose);

    /*
    ** Kills a CGI process, cleans up both pipe fds and _pipeToClient
    ** entries, and resets ctx.  Called from timeout cleanup and error paths.
    */
    void abort_Cgi(Client &client, std::vector<pollfd> &fds);

  public:
    Server(int port, ServerConfig &sc);
    ~Server();

    sockaddr_in create_Address();
    int         setup_Socket();
    int         accept_NewClient(std::vector<pollfd> &fds);
    bool        receive_FromClient(std::vector<pollfd> &fds, size_t index);
    SendStatus  send_ToClient(std::vector<pollfd> &fds, size_t index);
    void        cleanup_TimeoutClients(std::vector<pollfd> &fds, time_t now,
                                       int timeoutSec);

    static void build_PollList(std::vector<Server> &servers,
                               std::vector<pollfd> &fds);
    static bool try_AcceptClient(std::vector<Server> &servers,
                                 std::vector<pollfd> &fds, int fd);
    static bool process_ClientRead(std::vector<Server> &servers,
                                   std::vector<pollfd> &fds, size_t i);
    static bool process_ClientWrite(std::vector<Server> &servers,
                                    std::vector<pollfd> &fds, size_t i);

    /*
    ** New static dispatchers for CGI pipe events.
    ** They iterate over servers, look up the pipe fd in _pipeToClient,
    ** and delegate to the instance method.
    */
    static bool dispatch_CgiWrite(std::vector<Server> &servers,
                                  std::vector<pollfd> &fds, size_t i);
    static bool dispatch_CgiRead(std::vector<Server> &servers,
                                 std::vector<pollfd> &fds, size_t i);

    static void close_AllClients(std::vector<Server> &servers);
    static void handle_Clients(std::vector<Server> &servers);
};

void handle_Sigint(int sig);

#endif
