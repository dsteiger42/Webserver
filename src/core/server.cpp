/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/24 14:11:24 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <core/server.hpp>
#include <http/request/Request.hpp>
#include <http/routing/Router.hpp>
#include <utils/signals/signals.hpp>

Server::Server(int port, ServerConfig &sc) : _server_fd(-1), _port(port),
	_router(sc)
{
}

Server::~Server()
{
	if (_server_fd != -1)
		close(_server_fd);
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

int Server::accept_NewClient(std::vector<pollfd> &fds, size_t tick)
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
	_allClients[client_fd].lastActivityTick  = tick;
    _allClients[client_fd].requestStartTick  = tick; 
	return (client_fd);
}

bool Server::receive_FromClient(std::vector<pollfd> &fds, size_t index, size_t tick)
{
	int		client_fd;
	int		bytes_received;
	char	buffer[1024];

	client_fd = fds[index].fd;
	Client &client = _allClients[client_fd];
	bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_received > 0)
	{
		if (client.drain)
			return (true); // discard data, wait for client to close
		client.lastActivityTick = tick;
		std::string chunk(buffer, bytes_received);
		std::cout << "Client " << client_fd << ": " << chunk << "\n";
		client.request.fill_Buffer(chunk, chunk.size());
		while (client.request.is_Done() || (!client.request.get_validRequest()
				&& client.request.get_statusCode() != 0))
		{
			
            client.response = _router.handle_Request(client.request, &client.cgi);
			if (client.cgi.active)
            {
                // CGI assíncrono iniciado
                client.cgi.clientFd  = client_fd;
                client.cgi.startTick = tick;
                // Adicionar outPipe ao poll (leitura)
                pollfd cgi_out;
                std::memset(&cgi_out, 0, sizeof(cgi_out));
                cgi_out.fd     = client.cgi.outPipeFd;
                cgi_out.events = POLLIN;
                fds.push_back(cgi_out);
                _pipeToClient[client.cgi.outPipeFd] = client_fd;
                // Se há body para enviar, adicionar inPipe ao poll (escrita)
                if (client.cgi.inPipeFd != -1)
                {
                    pollfd cgi_in;
                    std::memset(&cgi_in, 0, sizeof(cgi_in));
                    cgi_in.fd     = client.cgi.inPipeFd;
                    cgi_in.events = POLLOUT;
                    fds.push_back(cgi_in);
                    _pipeToClient[client.cgi.inPipeFd] = client_fd;
                }
            }
            else
            {
                // Resposta imediata (não-CGI)
				if (client.response.get_StatusCode() == 413)
        			client.drain = true;
                std::string raw = client.response.serialize();
                client.writeBuffer.write(raw.c_str(), raw.size());
                fds[index].events |= POLLOUT;
            }

            std::string leftover = client.request.get_Leftover();
            client.request.reset();
            if (leftover.empty())
                break;
            client.request.fill_Buffer(leftover, leftover.size());
        }
        return true;
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

void Server::cleanup_TimeoutClients(std::vector<pollfd> &fds, size_t tick)
{
    const size_t INCOMPLETE_TIMEOUT = 5;   // ~5 ticks ≈ 5s
    const size_t CLIENT_TIMEOUT     = 30;  // ~30 ticks ≈ 30s
    const size_t CGI_TIMEOUT        = 30;  // ~30 ticks ≈ 30s

    std::map<int, Client>::iterator it = _allClients.begin();
    while (it != _allClients.end())
    {
        int     fd     = it->first;
        Client &client = it->second;
        bool    doClose = false;
        std::string errorResponse;

        // Timeout de CGI em curso
        if (client.cgi.active &&
            tick - client.cgi.startTick > CGI_TIMEOUT)
        {
            kill(client.cgi.pid, SIGKILL);
            waitpid(client.cgi.pid, NULL, WNOHANG);
            // Limpar outPipe do poll
            for (size_t i = 0; i < fds.size(); i++)
                if (fds[i].fd == client.cgi.outPipeFd)
                { close(client.cgi.outPipeFd); 
						_pipeToClient.erase(client.cgi.outPipeFd); 
						fds.erase(fds.begin() + i); 
						break; 
				}
            // Fechar inPipe se ainda aberto
            if (client.cgi.inPipeFd != -1)
            {
                for (size_t i = 0; i < fds.size(); i++)
                    if (fds[i].fd == client.cgi.inPipeFd)
                        { close(client.cgi.inPipeFd); _pipeToClient.erase(client.cgi.inPipeFd); fds.erase(fds.begin() + i); break; }
            }
            // Enfileirar 504 Gateway Timeout
            Response r = _router.make_ErrorCode(504);
            std::string raw = r.serialize();
            client.writeBuffer.write(raw.c_str(), raw.size());
            client.cgi.active = false;
            for (size_t i = 0; i < fds.size(); i++)
                if (fds[i].fd == fd) { fds[i].events |= POLLOUT; break; }
            ++it;
            continue;
        }
        // Timeout de request incompleto
        if (!client.request.is_Done() && !client.cgi.active)
        {
            if (tick - client.requestStartTick > INCOMPLETE_TIMEOUT)
            {
                // Enfileirar 408 no writeBuffer (não enviar directo — respeitar o poll)
                std::string r408 = "HTTP/1.1 408 Request Timeout\r\n"
                                   "Content-Type: text/plain\r\n"
                                   "Content-Length: 15\r\n"
                                   "Connection: close\r\n\r\n"
                                   "Request Timeout";
                client.writeBuffer.write(r408.c_str(), r408.size());
                for (size_t i = 0; i < fds.size(); i++)
                    if (fds[i].fd == fd) { fds[i].events = POLLOUT; break; }
                doClose = true;
            }
        }
        else if (!client.cgi.active)
        {
            if (tick - client.lastActivityTick > CLIENT_TIMEOUT)
                doClose = true;
        }

        if (doClose)
        {
            close(fd);
            for (size_t i = 0; i < fds.size(); i++)
                if (fds[i].fd == fd) { fds.erase(fds.begin() + i); break; }
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
	bool		isCgiPipe;
	pollfd		listen_fd;
	SendStatus	status;
	size_t		tickCount;
	const int	POLL_TIMEOUT_MS  = 1000;
	/* const int	CLIENT_TIMEOUT_SEC = 30; */
	int			fd;

	tickCount = 0;
	std::vector<pollfd> fds;
	for (size_t s = 0; s < servers.size(); s++)
	{
		listen_fd.fd     = servers[s]._server_fd;
		listen_fd.events = POLLIN;
		fds.push_back(listen_fd);
	}
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
		tickCount++;
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents == 0)
				continue ;

			// --- CGI pipe ---
			isCgiPipe = false;
			for (size_t s = 0; s < servers.size(); s++)
			{
				if (servers[s]._pipeToClient.count(fds[i].fd))
				{
					isCgiPipe = true;
					if (fds[i].events & POLLOUT)
						servers[s].handle_CgiWrite(fds, i);
					else
						servers[s].handle_CgiRead(fds, i, tickCount);
					break ;
				}
			}
			if (isCgiPipe)
			{
				i--;
				continue ;
			}

			// --- POLLIN ---
			if (fds[i].revents & POLLIN)
			{
				is_server_fd = false;
				for (size_t s = 0; s < servers.size(); s++)
				{
					if (fds[i].fd == servers[s]._server_fd)
					{
						servers[s].accept_NewClient(fds, tickCount);
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
							if (!servers[s].receive_FromClient(fds, i, tickCount))
								i--;
							break ;
						}
					}
				}
			}

			// --- POLLOUT ---
			if (i < fds.size() && fds[i].revents & POLLOUT)
			{
				for (size_t s = 0; s < servers.size(); s++)
				{
					if (servers[s]._allClients.count(fds[i].fd))
					{
						status = servers[s].send_ToClient(fds, i);
						if (status == SEND_DONE)
						{
							fd = fds[i].fd;
							Client &c = servers[s]._allClients[fd];
							if (c.response.get_StatusCode() == 413)
							{
								// Fechar com linger=0 para evitar RST enquanto há dados
								// não lidos no kernel buffer — o TCP envia FIN limpo
								c.drain = true;
								fds[i].events = POLLIN;
							}
							else
							{
                                // Normal case: body was fully read, safe to close immediately.
								close(fd);
								servers[s]._allClients.erase(fd);
								fds.erase(fds.begin() + i);
								i--;
							}
						}
						break ;
					}
				}
			}
		}
		for (size_t s = 0; s < servers.size(); s++)
			servers[s].cleanup_TimeoutClients(fds, tickCount);
	}
}


// POLLOUT no inPipeFd — escrever body ao CGI
void Server::handle_CgiWrite(std::vector<pollfd> &fds, size_t index)
{
    int pipeFd   = fds[index].fd;
    int clientFd = _pipeToClient[pipeFd];
    if (!_allClients.count(clientFd))
    {
        close(pipeFd);
        _pipeToClient.erase(pipeFd);
        fds.erase(fds.begin() + index);
        return;
    }
    CGIPending &cgi = _allClients[clientFd].cgi;
    const std::string &body = cgi.bodyToWrite;
    size_t remaining = body.size() - cgi.bodyWritten;

    ssize_t n = write(pipeFd, body.c_str() + cgi.bodyWritten, remaining);
    if (n > 0)
        cgi.bodyWritten += n;

    // Quando tudo foi escrito (ou erro) — fechar stdin do CGI
    if (n <= 0 || cgi.bodyWritten >= body.size())
    {
        close(pipeFd);
        cgi.inPipeFd = -1;
        _pipeToClient.erase(pipeFd);
        fds.erase(fds.begin() + index);
    }
}

// POLLIN/POLLHUP no outPipeFd — ler output do CGI
void Server::handle_CgiRead(std::vector<pollfd> &fds, size_t index, size_t tick)
{
    int pipeFd   = fds[index].fd;
    int clientFd = _pipeToClient[pipeFd];
    if (!_allClients.count(clientFd))
    {
        close(pipeFd);
        _pipeToClient.erase(pipeFd);
        fds.erase(fds.begin() + index);
        return;
    }
    CGIPending &cgi = _allClients[clientFd].cgi;

    if (fds[index].revents & POLLIN)
    {
        char   temp[4096];
        ssize_t n = read(pipeFd, temp, sizeof(temp));
        if (n > 0)
        {
            if (cgi.outputBuffer.size() + n > MAX_CGI_OUTPUT)
            {
                // Output demasiado grande — matar o processo
                kill(cgi.pid, SIGKILL);
                waitpid(cgi.pid, NULL, WNOHANG);
                finalize_Cgi(clientFd, fds);
                return;
            }
            cgi.outputBuffer.append(temp, n);
            return; // aguardar mais dados
        }
        if (n == 0 || (n == -1 && errno != EAGAIN && errno != EWOULDBLOCK))
        {
            // EOF ou erro real — CGI terminou
            finalize_Cgi(clientFd, fds);
        }
        // n == -1 && EAGAIN: poll voltará a notificar
    }
    else if (fds[index].revents & (POLLHUP | POLLERR))
    {
        // Ler o que restar antes de finalizar
        char   temp[4096];
        ssize_t n;
        while ((n = read(pipeFd, temp, sizeof(temp))) > 0)
        {
            if (cgi.outputBuffer.size() + n <= MAX_CGI_OUTPUT)
                cgi.outputBuffer.append(temp, n);
        }
        finalize_Cgi(clientFd, fds);
    }
    (void)tick;
}

void Server::finalize_Cgi(int clientFd, std::vector<pollfd> &fds)
{
    if (!_allClients.count(clientFd))
        return;
    Client    &client = _allClients[clientFd];
    CGIPending &cgi   = client.cgi;

    // Recolher o exit status sem bloquear
    waitpid(cgi.pid, &cgi.waitStatus, WNOHANG);

    // Fechar e remover outPipe do poll
    for (size_t i = 0; i < fds.size(); i++)
    {
        if (fds[i].fd == cgi.outPipeFd)
        {
            close(cgi.outPipeFd);
            _pipeToClient.erase(cgi.outPipeFd);
            fds.erase(fds.begin() + i);
            break;
        }
    }
    cgi.outPipeFd = -1;
    cgi.active    = false;

    // Construir a Response
    CGI &cgiObj = *_router.cgi;
    Response res;
    if (!WIFEXITED(cgi.waitStatus) || WEXITSTATUS(cgi.waitStatus) != 0
        || !cgiObj.is_ValidCGIOutput(cgi.outputBuffer))
    {
        res = _router.make_ErrorCode(502);
    }
    else
    {
        CGI::CGIResult result = cgiObj.parse_CGIOutput(cgi.outputBuffer);
        res.set_StatusCode(result.status);
        res.set_Header("Content-Type", result.contentType);
        if (result.headers.count("Location"))
            res.set_Header("Location", result.headers.at("Location"));
        res.set_Body(result.body);
    }

    std::string raw = res.serialize();
    client.writeBuffer.write(raw.c_str(), raw.size());

    // Activar POLLOUT no socket do cliente
    for (size_t i = 0; i < fds.size(); i++)
    {
        if (fds[i].fd == clientFd)
        {
            fds[i].events |= POLLOUT;
            break;
        }
    }
}