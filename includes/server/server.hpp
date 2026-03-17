#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <unistd.h>     // execve, fork, pipe, dup, dup2, chdir, access, close, read, write
#include <sys/wait.h>   // waitpid
#include <signal.h>     // signal, kill
#include <errno.h>      // errno
#include <string.h>    // strerror
#include <netdb.h>     // gai_strerror
#include <sys/socket.h> // socket, accept, listen, bind, connect, socketpair, send, recv, setsockopt, getsockname
#include <netinet/in.h>// htons, htonl, ntohs, ntohl
#include <netdb.h>     // getaddrinfo, freeaddrinfo, getprotobyname
#include <arpa/inet.h>// (sometimes needed for htons/inet helpers)
#include <sys/select.h> // select
#include <poll.h>       // poll
#include <sys/epoll.h> // epoll
#include <sys/epoll.h>  // epoll_create, epoll_ctl, epoll_wait   (Linux)
#include <fcntl.h>      // open, fcntl
#include <sys/stat.h>  // stat
#include <dirent.h>    // opendir, readdir, closedir
#include <vector>
#include <map>
#include <client/client.hpp>
#include <http/Router.hpp>


enum SendStatus
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
        std::map<int, Client> _allClients; // store each fd to each client
        Router _router; //Rafael  

    public:
        Server(int port, const t_parser &parser);
        ~Server();
        sockaddr_in create_address();
        int setup_socket();
        int accept_new_client(std::vector<pollfd> &fds);
        bool receive_from_client(std::vector<pollfd> &fds, size_t index);
        SendStatus send_to_client(std::vector<pollfd> &fds, size_t index);
        void handle_clients();
};

#endif