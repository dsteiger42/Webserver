/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/25 05:38:45 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <core/server.hpp>
#include <errno.h>
#include <http/cgi/CGI.hpp>
#include <http/request/Request.hpp>
#include <http/routing/Router.hpp>
#include <signal.h>
#include <utils/signals/signals.hpp>

// ============================================================
// Internal helpers
// ============================================================

static void	*ft_memset(void *str, int c, size_t n)
{
	unsigned char	*mem;

	mem = (unsigned char *)str;
	for (size_t i = 0; i < n; i++)
		mem[i] = (unsigned char)c;
	return (str);
}

static void	add_PollFd(std::vector<pollfd> &fds, int fd, short events)
{
	pollfd	pfd;

	ft_memset(&pfd, 0, sizeof(pfd));
	pfd.fd = fd;
	pfd.events = events;
	fds.push_back(pfd);
}

// ============================================================
// Constructor / Destructor
// ============================================================

Server::Server(int port, ServerConfig &sc) : _server_fd(-1), _port(port),
	_router(sc)
{
}

Server::~Server()
{
	if (_server_fd != -1)
		close(_server_fd);
}

// ============================================================
// Socket setup
// ============================================================

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
	int			flags;
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
	flags = fcntl(_server_fd, F_GETFL, 0);
	if (flags == -1 || fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
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

// ============================================================
// Client accept / receive / send
// ============================================================

int Server::accept_NewClient(std::vector<pollfd> &fds)
{
	pollfd		poll;
	sockaddr_in	client_addr;
	socklen_t	client_len;
	int			client_fd;
	int			flags;
	size_t		maxBody;

	client_len = sizeof(client_addr);
	client_fd = accept(_server_fd, (struct sockaddr *)&client_addr,
			&client_len);
	if (client_fd == -1)
	{
		std::cerr << "Error: accept failed\n";
		return (-1);
	}
	std::cout << "Client connected: fd=" << client_fd << "\n";
	flags = fcntl(client_fd, F_GETFL, 0);
	fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
	ft_memset(&poll, 0, sizeof(poll));
	poll.fd = client_fd;
	poll.events = POLLIN;
	fds.push_back(poll);
	_allClients[client_fd] = Client(client_fd);
	maxBody = _router.get_Config().config.client_max_body_size;
	_allClients[client_fd].request.set_MaxBodySize(maxBody);
	_allClients[client_fd].lastActivity = time(NULL);
	_allClients[client_fd].requestStart = time(NULL);
	return (client_fd);
}

bool Server::receive_FromClient(std::vector<pollfd> &fds, size_t index)
{
	int		client_fd;
	char	buffer[1024];
	int		bytes_received;

	client_fd = fds[index].fd;
	Client &client = _allClients[client_fd];
	bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received > 0)
	{
		if (client.drain)
			return (true);
		client.lastActivity = time(NULL);
		std::string chunk(buffer, bytes_received);
		std::cout << "Client " << client_fd << ": " << chunk << "\n";
		client.request.fill_Buffer(chunk, chunk.size());
		while (client.request.is_Done() || (!client.request.get_validRequest()
				&& client.request.get_statusCode() != 0))
		{
			// ---- CGI path -------------------------------------------
			// Ask the router whether this request targets a CGI location.
			// The router exposes a helper that checks the location without
			// executing anything.  If CGI is needed, launch asynchronously.
			if (_router.is_CgiRequest(client.request))
			{
				if (!start_Cgi(client, client.request, fds))
				{
					// launch failed: send 500 synchronously
					/*  client.response = _router.make_ErrorCode(500);
						std::string raw = client.response.serialize();
						client.writeBuffer.write(raw.c_str(), raw.size()); */
					fds[index].events |= POLLOUT;
				}
				// Regardless of success/failure the request is consumed.
				// Do NOT reset here: the CGI context holds bodyToSend which
				// was copied from req.get_Body() inside launch().
				client.request.reset();
				break ;
			}
			client.response = _router.handle_Request(client.request);
			std::string raw = client.response.serialize();
			client.writeBuffer.write(raw.c_str(), raw.size());
			fds[index].events |= POLLOUT;
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
			std::cout << "Client disconnected: fd=" << client_fd << "\n";
		else
			std::cerr << "Error receiving from client fd=" << client_fd << "\n";
		// If a CGI was in flight for this client, abort it cleanly.
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

// ============================================================
// CGI async helpers (instance methods)
// ============================================================

bool Server::start_Cgi(Client &client, const Request &req,
	std::vector<pollfd> &fds)
{
	int	result;

	CgiContext &ctx = client.cgi;
	Location &loc = _router.matchLocation(req.get_Path());
	result = _router.cgi->launch(req, loc, ctx);
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

/*
** FIX Bug 4 — process_CgiWrite
**
** O write() num fd O_NONBLOCK pode retornar -1 com errno == EAGAIN
** quando o pipe está cheio.  Isso NÃO é um erro fatal — significa
** "tenta mais tarde".  O poll() voltará a disparar POLLOUT quando
** houver espaço no pipe.
**
** Só fechamos inFd quando:
**   a) todos os bytes foram enviados (bodyOffset >= bodyToSend.size())
**   b) write() retornou um erro real (!= EAGAIN && != EINTR)
*/
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
        // If all bytes sent, close inFd to signal EOF to CGI stdin
        if (ctx.bodyOffset >= ctx.bodyToSend.size())
        {
            remove_PipeFd(fds, pipeFd, true);
            ctx.inFd = -1;
        }
        return;
    }

    if (written == -1 && (errno == EAGAIN || errno == EINTR))
        return; // Pipe full or interrupted — poll() will retry

    // Real write error: close pipe, CGI stdin gets EOF unexpectedly.
    // The CGI process will likely exit with an error; process_CgiRead
    // will handle the rest when outFd fires or times out.
    remove_PipeFd(fds, pipeFd, true);
    ctx.inFd = -1;
}

/*
** FIX Bug 3 — process_CgiRead
**
** Com O_NONBLOCK, read() pode retornar -1 com errno == EAGAIN quando
** não há dados disponíveis no pipe.  Isso NÃO é EOF — o CGI ainda
** está a correr.  Só tratamos como EOF quando n == 0.
**
** Tabela de comportamento:
**   n > 0             → dados lidos, acumula em ctx.output, continua
**   n == -1 && EAGAIN → sem dados agora, poll() voltará quando houver
**   n == -1 && EINTR  → interrupção por sinal, ignora e continua
**   n == 0            → EOF real, CGI fechou stdout, processa resposta
**   n == -1 (outro)   → erro real do pipe, aborta com 502
*/
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
		// Output too large: abort and send 502
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
		return ; // More data may come; stay in poll()
	}
	// FIX: distinguish EAGAIN/EINTR from real EOF
	if (n == -1)
	{
		if (errno == EAGAIN || errno == EINTR)
			return ; // No data right now — poll() will fire again
		// Real read error: treat as bad CGI output
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
	// If inFd still open (e.g. CGI exited before we finished writing),
	// close it now so we don't leak.
	if (ctx.inFd != -1)
	{
		remove_PipeFd(fds, ctx.inFd, true);
		ctx.inFd = -1;
	}
	// Collect child exit status
	waitStatus = 0;
	waitpid(ctx.pid, &waitStatus, 0);
	// Build HTTP response from accumulated CGI output
	client.response = _router.cgi->finish(ctx, waitStatus);
	ctx.reset();
	std::string raw = client.response.serialize();
	client.writeBuffer.write(raw.c_str(), raw.size());
	// Arm POLLOUT on the client fd
	for (size_t j = 0; j < fds.size(); j++)
	{
		if (fds[j].fd == clientFd)
		{
			fds[j].events |= POLLOUT;
			break ;
		}
	}
}

// ============================================================
// Timeout cleanup
// ============================================================

void Server::cleanup_TimeoutClients(std::vector<pollfd> &fds, time_t now,
	int timeoutSec)
{
	const int	INCOMPLETE_REQUEST_TIMEOUT_SEC = 5;
	int			fd;
	bool		doKill;

	std::map<int, Client>::iterator it = _allClients.begin();
	while (it != _allClients.end())
	{
		fd = it->first;
		Client &client = it->second;
		doKill = false;
		// --- CGI timeout ---
		if (client.cgi.active && (now - client.cgi.startTime) > CGI_TIMEOUT_SEC)
		{
			std::cout << "CGI timeout for client " << fd << "\n";
			abort_Cgi(client, fds);
			client.response = _router.make_ErrorCode(504);
			std::string raw = client.response.serialize();
			client.writeBuffer.write(raw.c_str(), raw.size());
			for (size_t i = 0; i < fds.size(); i++)
			{
				if (fds[i].fd == fd)
				{
					fds[i].events |= POLLOUT;
					break ;
				}
			}
			++it;
			continue ;
		}
		// --- Incomplete request timeout ---
		if (!client.request.is_Done() && !client.cgi.active)
		{
			if (now - client.requestStart > INCOMPLETE_REQUEST_TIMEOUT_SEC)
			{
				std::cout << "Client " << fd << " timed out (incomplete request)\n";
				std::string response408 = "HTTP/1.1 408 Request Timeout\r\n"
											"Content-Type: text/plain\r\n"
											"Content-Length: 15\r\n"
											"Connection: close\r\n"
											"\r\n"
											"Request Timeout";
				send(fd, response408.c_str(), response408.size(), 0);
				doKill = true;
			}
		}
		// --- General inactivity timeout ---
		else if (!client.cgi.active && (now - client.lastActivity) > timeoutSec)
		{
			doKill = true;
		}
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

// ============================================================
// Static helpers
// ============================================================

void Server::build_PollList(std::vector<Server> &servers,
	std::vector<pollfd> &fds)
{
	for (size_t i = 0; i < servers.size(); i++)
		add_PollFd(fds, servers[i]._server_fd, POLLIN);
}

bool Server::try_AcceptClient(std::vector<Server> &servers,
	std::vector<pollfd> &fds, int fd)
{
	for (size_t s = 0; s < servers.size(); s++)
	{
		if (fd == servers[s]._server_fd)
		{
			servers[s].accept_NewClient(fds);
			return (true);
		}
	}
	return false;
}

bool Server::process_ClientRead(std::vector<Server> &servers,
	std::vector<pollfd> &fds, size_t i)
{
	for (size_t s = 0; s < servers.size(); s++)
	{
		if (servers[s]._allClients.count(fds[i].fd))
			return servers[s].receive_FromClient(fds, i);
	}
	return true;
}

bool Server::process_ClientWrite(std::vector<Server> &servers,
	std::vector<pollfd> &fds, size_t i)
{
	int			fd;
	SendStatus	status;

	for (size_t s = 0; s < servers.size(); s++)
	{
		if (!servers[s]._allClients.count(fds[i].fd))
			continue ;
		fd = fds[i].fd;
		status = servers[s].send_ToClient(fds, i);
		if (status == SEND_OK)
			return true;
		if (status == SEND_DONE)
		{
			Client &c = servers[s]._allClients[fd];
			if (c.response.get_StatusCode() == 413)
			{
				c.drain = true;
				fds[i].events = POLLIN;
				return true;
			}
			close(fd);
			servers[s]._allClients.erase(fd);
			fds.erase(fds.begin() + i);
			return false;
		}
		close(fd);
		servers[s]._allClients.erase(fd);
		fds.erase(fds.begin() + i);
		return false;
	}
	return true;
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
	const int POLL_TIMEOUT_MS = 1000;
	const int CLIENT_TIMEOUT_SEC = 30;

	std::vector<pollfd> fds;
	build_PollList(servers, fds);

	while (g_running)
	{
		int ret = poll(fds.data(), fds.size(), POLL_TIMEOUT_MS);
		if (ret == -1)
		{
			if (errno == EINTR)
				continue ;
			std::cerr << "Error: poll failed\n";
			break ;
		}
		time_t now = time(NULL);
		for (size_t i = 0; i < fds.size();)
		{
			short revents = fds[i].revents;
			int fd = fds[i].fd;

			// Nothing fired on this fd
			if (revents == 0)
			{
				++i;
				continue ;
			}
			// --- 1. Server listen socket: new connection ---
			// FIX Bug 1: only call try_AcceptClient when POLLIN fires
			// AND the fd is actually a server socket.
			if ((revents & POLLIN) && try_AcceptClient(servers, fds, fd))
			{
				++i;
				continue ;
			}
			// --- Classify the fd before dispatching ---
			// Check if this fd belongs to a CGI pipe (present in any
			// server's _pipeToClient map).  If so, route to the CGI
			// handlers exclusively — never to the client handlers.
			bool isCgiPipe = false;
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
				// FIX Bug 2: pipe fds are routed here and NEVER reach
				// process_ClientRead / process_ClientWrite below.
				if (revents & POLLOUT)
					dispatch_CgiWrite(servers, fds, i);
				// Re-check bounds: dispatch_CgiWrite may have erased fds[i]
				if (i < fds.size() && fds[i].fd == fd && (revents & POLLIN))
					dispatch_CgiRead(servers, fds, i);
				// After CGI dispatch, always restart the loop from current i.
				// The fd may have been removed; if it was, the next element
				// is now at position i automatically.
				continue ;
			}
			// --- 4. Client socket: read ---
			if (revents & POLLIN)
			{
				if (!process_ClientRead(servers, fds, i))
					continue ;
			}
			// --- 5. Client socket: write ---
			if (i < fds.size() && fds[i].fd == fd && (revents & POLLOUT))
			{
				if (!process_ClientWrite(servers, fds, i))
					continue ;
			}
			++i;
		}
		for (size_t s = 0; s < servers.size(); s++)
			servers[s].cleanup_TimeoutClients(fds, now, CLIENT_TIMEOUT_SEC);
	}
	close_AllClients(servers);
}