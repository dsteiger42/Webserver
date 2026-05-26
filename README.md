# 🌐 Webserv

> A fully functional HTTP/1.1 web server written in C++98 from scratch — built as part of the **42 Lisboa** curriculum.

![Language](https://img.shields.io/badge/language-C++98-00599C?style=flat-square&logo=cplusplus)
![School](https://img.shields.io/badge/school-42_Lisboa-black?style=flat-square)
![Protocol](https://img.shields.io/badge/protocol-HTTP%2F1.1-blue?style=flat-square)
![I/O](https://img.shields.io/badge/I%2FO-poll()-orange?style=flat-square)

---

## 📖 About

**Webserv** is an HTTP/1.1 web server implemented entirely in C++98 with no external libraries. The goal is to understand how the web works at a low level — implementing sockets, request parsing, response handling, CGI execution, and connection management from scratch.

All I/O multiplexing runs through a **single global `poll()` call** — no threads, no blocking, no nested event loops.

---

## ✨ Features

### 🔌 Core Server
- **TCP socket server** — handles multiple concurrent client connections
- **Single `poll()` event loop** — all sockets (listen, client, CGI pipes) share one `pollfd` array
- **HTTP pipelining** — multiple requests per connection via `CircularBuffer` with leftover handling
- **Multiple server blocks** — different ports/addresses in one config file
- **Signal handling** — graceful shutdown on `SIGINT`; `SIGPIPE` suppressed via `sigaction`

### 📨 HTTP Protocol
- **HTTP/1.1 request parser** — state machine supporting fragmented TCP delivery
- **Supported methods** — `GET`, `POST`, `DELETE`
- **Chunked transfer encoding** — full support for chunked request bodies
- **Static file serving** — with automatic MIME type detection
- **Directory listing** — `autoindex` with XSS-safe HTML generation
- **Custom error pages** — configurable per server block
- **Drain-before-close** — graceful teardown on 413 responses (avoids TCP RST)

### ⚙️ CGI Execution
- **Fully async CGI** — `fork/execve` integrated into the `poll()` loop (no blocking `waitpid`)
- **Pipe-based I/O** — stdin/stdout streamed through `poll()` events
- **CGI timeout** — enforced via tick counter (no `time()` call)
- **Process isolation** — CGI runs in a child process; server is unaffected by script crashes
- **Supported** — Python (`.py`), PHP (`.php` via `php-cgi`), Bash (`.sh`), and any configured interpreter

### 🗂️ Configuration
- **NGINX-inspired syntax** — `server` and `location` blocks
- Full directive support: `listen`, `server_name`, `root`, `index`, `autoindex`, `allowed_methods`, `client_max_body_size`, `return`, `error_page`, `upload_store`
- **CGI block** — map file extensions to interpreter paths
- **MIME block** — override or extend the built-in type table

---

## 🗂️ Project Structure

```
Webserver/
├── includes/               # All .hpp headers
├── src/
│   ├── core/               # Server event loop, client state machine
│   ├── http/               # Request parser, response builder, CircularBuffer
│   ├── cgi/                # CGI launch, env builder, response parser
│   ├── config/parser/      # Config file parser (server, location, cgi, mime, error)
│   └── utils/              # Filesystem, routing, autoindex, MIME, signals
├── www/                    # Default web root (HTML, error pages, CGI scripts)
├── main.cpp                # Entry point
├── webserver.conf          # Default configuration file
└── Makefile
```

---

## ⚙️ Configuration File

NGINX-inspired syntax. Blocks delimited by `{ }`. Lines starting with `#` are comments.

```nginx
server {
    listen                8080
    server_name           localhost
    root                  ./www
    index                 index.html
    client_max_body_size  10M

    error_page  404  ./www/errors/404.html
    error_page  500  ./www/errors/500.html
    error_page  413  ./www/errors/413.html

    location / {
        allowed_methods  GET
        autoindex        off
    }

    location /upload {
        allowed_methods  POST DELETE
        upload_store     ./www/uploads
    }

    location /cgi-bin {
        allowed_methods  GET POST
    }

    location /old {
        return  301  /
    }

    cgi {
        .py   /usr/bin/python3
        .php  /usr/bin/php-cgi
        .sh   /bin/bash
    }

    mime {
        .html  text/html
        .css   text/css
        .js    application/javascript
        .png   image/png
        .jpg   image/jpeg
        .json  application/json
        .pdf   application/pdf
    }
}
```

### Directives Reference

| Directive | Scope | Description |
|---|---|---|
| `listen` | server | Port (`8080`) or address:port (`0.0.0.0:8080`) |
| `server_name` | server | Hostname label |
| `root` | server / location | Document root |
| `index` | server / location | Default index file |
| `client_max_body_size` | server / location | Max body size — units: `B` `K` `M` `G` |
| `error_page` | server | `<code> <path>` — custom HTML error page |
| `autoindex` | location | `on` / `off` — directory listing |
| `allowed_methods` | location | `GET` `POST` `DELETE` (space-separated) |
| `upload_store` | location | Target directory for POST file uploads |
| `return` | location | `<code> <url>` — HTTP redirect |
| `cgi` block | server | `.<ext> <interpreter>` mappings |
| `mime` block | server | `.<ext> <Content-Type>` mappings |

---

## 🚀 Getting Started

```bash
git clone https://github.com/1Fr3aK2/Webserver.git
cd Webserver
make
```

```bash
# With default config
./Webserv

# With custom config
./Webserv path/to/your.conf
```

### Makefile Targets

| Target | Description |
|---|---|
| `make` | Build the project |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and binary |
| `make re` | Full rebuild |
| `make v` | Run with Valgrind (leak + FD check) |

---

## 🧪 Testing

```bash
# Browser
http://localhost:8080/
http://localhost:8080/cgi-bin/test.py

# Netcat (raw HTTP)
nc localhost 8080
GET / HTTP/1.1
Host: localhost

# curl
curl -v http://localhost:8080/
curl -v -X POST -F "file=@test.txt" http://localhost:8080/upload
curl -v -X DELETE http://localhost:8080/upload/test.txt
```

---

## 🧠 Design Decisions

**TCP over UDP** — ordered, reliable delivery is required by HTTP. Used in virtually every real-world web server.

**HTTP/1.1 over HTTP/2 / HTTP/3** — text-based design is practical to implement at a low level. HTTP/2 and HTTP/3 introduce binary framing and QUIC/TLS integration that goes beyond the project scope.

**`poll()` over `select()` / `epoll()`** — no 1024 FD cap like `select()`, simpler API than `epoll()`'s create/ctl/wait lifecycle, and portable across Linux and macOS.

**Async CGI over blocking** — CGI pipes are registered in the global `poll()` array. No blocking `waitpid()`, no nested `poll()` calls — full compliance with the 42 subject rules.

---

## 👥 Authors

- **[1F3aK2](https://github.com/1Fr3aK2)** & **[dsteiger](https://github.com/dsteiger42)**

---

> *Built at 42 Lisboa — C++98 — `poll()` — HTTP/1.1*
