# 🌐 Webserv

> A fully functional HTTP/1.1 web server written in C++ from scratch — built as part of the **42 School** curriculum.

![Language](https://img.shields.io/badge/language-C++-00599C?style=flat-square&logo=cplusplus)
![School](https://img.shields.io/badge/school-42-black?style=flat-square)
![Protocol](https://img.shields.io/badge/protocol-HTTP%2F1.1-blue?style=flat-square)
![Norm](https://img.shields.io/badge/norminette-compliant-brightgreen?style=flat-square)

---

## 📖 About

**Webserv** is an HTTP/1.1 web server implemented entirely in C++ with no external libraries. The goal is to understand how the web works at a low level — implementing sockets, request parsing, response handling, CGI execution, and connection management from scratch.

Security was applied throughout: path traversal prevention, request validation before any file system access, and process isolation for CGI execution via `fork/execve`.

---

## ✨ Features

### 🔌 Core Server
- **TCP socket server** — accepts and handles multiple concurrent client connections
- **`poll()`-based I/O multiplexing** — monitors all connected clients without blocking, with no arbitrary file descriptor limit
- **Persistent connections** — supports `keep-alive` for HTTP/1.1
- **Multiple server blocks** — one config file can define multiple virtual hosts on different ports
- **Signal handling** — graceful shutdown on `SIGINT`; `SIGPIPE` suppressed

### 📨 HTTP Protocol
- **HTTP/1.1 request parser** — parses method, URI, headers, and body
- **Supported methods** — `GET`, `POST`, `DELETE`
- **Static file serving** — serves files with automatic MIME type detection
- **Custom error pages** — configurable per server block
- **Status codes** — correct responses for 200, 201, 204, 301, 400, 403, 404, 405, 413, 500, etc.

### ⚙️ CGI Execution
- **CGI support** — executes external scripts via `fork/execve`
- **Pipe-based output capture** — captures CGI stdout and sends it as the HTTP response body
- **Process isolation** — CGI runs in a child process, fully isolated from the server

### 🔐 Security
- **Path traversal prevention** — all requested paths validated before any file system access
- **Request validation** — malformed or oversized requests rejected before processing
- **Process isolation for CGI** — script execution never runs in the server process

### 🗂️ Configuration
- **Config file parser** — NGINX-inspired syntax with support for:
  - Virtual hosts (`server` blocks)
  - `listen` — port binding
  - `server_name` — hostname matching
  - `root` — document root
  - `index` — default index file
  - `location` blocks — per-path routing rules
  - `allowed_methods` — restrict HTTP methods per location
  - `client_max_body_size` — request body size limit
  - `return` — HTTP redirections
  - `cgi_pass` — CGI script association by extension
  - `autoindex` — directory listing toggle
  - `error_page` — custom error pages

---

## 🗂️ Project Structure

```
Webserver/
├── includes/
│   ├── config/
│   │   └── parser/         # Config file parser
│   ├── core/
│   │   └── server.hpp      # Server class
│   └── utils/
│       └── signals/        # Signal handlers
├── src/                    # Implementation files
├── www/                    # Default web root (HTML, CGI scripts)
├── main.cpp                # Entry point
├── webserver.conf          # Default configuration file
└── Makefile
```

---

## ⚙️ Configuration File

The server is configured via a `.conf` file inspired by NGINX syntax. Example:

```nginx
server {
    listen       8080;
    server_name  localhost;
    root         ./www;
    index        index.html;
    client_max_body_size 10M;

    error_page 404 /errors/404.html;
    error_page 500 /errors/500.html;

    location / {
        allowed_methods GET POST;
        autoindex off;
    }

    location /uploads {
        allowed_methods GET POST DELETE;
        autoindex on;
    }

    location /cgi-bin {
        allowed_methods GET POST;
        cgi_pass .py;
    }

    location /old-page {
        return 301 /new-page;
    }
}
```

---

## 🚀 Getting Started

### Prerequisites

**Ubuntu / Debian**

```bash
sudo apt update
sudo apt install -y g++ make
```

**macOS**

```bash
xcode-select --install
```

### Build

```bash
git clone https://github.com/1Fr3aK2/Webserver.git
cd Webserver
make
```

### Run

```bash
# With default config
./webserv

# With custom config
./webserv path/to/your.conf
```

---

## 🧹 Makefile Targets

| Target        | Description                            |
|---------------|----------------------------------------|
| `make`        | Build the project                      |
| `make clean`  | Remove object files                    |
| `make fclean` | Remove object files and the executable |
| `make re`     | Full rebuild (`fclean` + `all`)        |

---

## 🧪 Testing

**Test with a browser:**
```
http://localhost:8080
```

**Test with netcat:**
```bash
nc localhost 8080
GET / HTTP/1.1
Host: localhost
```

**Test CGI:**
```
http://localhost:8080/cgi-bin/test.py
```

**Test with curl:**
```bash
# GET request
curl http://localhost:8080/

# POST request
curl -X POST -d "data=hello" http://localhost:8080/upload

# DELETE request
curl -X DELETE http://localhost:8080/uploads/file.txt
```

---

## 🧠 Design Decisions

**TCP over UDP** — TCP guarantees ordered, reliable packet delivery, which is required by HTTP. Used in virtually every real-world web server.

**HTTP/1.1 over HTTP/2 or HTTP/3** — HTTP/1.1's simple text-based design is practical to implement and debug at a low level. HTTP/2 and HTTP/3 introduce binary framing and QUIC/TLS integration that goes beyond the scope of this project.

**`poll()` over `select()` and `epoll()`** — `poll()` has no arbitrary file descriptor limit like `select()`'s 1024 cap, and is simpler than `epoll()`'s create/ctl/wait lifecycle. The O(n) scan cost is acceptable for moderate connection counts, and `poll()` is portable across Linux, macOS, and BSD.

**`fork/execve` for CGI** — running CGI scripts in a child process fully isolates them from the server. A crash or exploit in a CGI script cannot affect the server process.

---

## 👥 Authors

- **[1Fr3aK2](https://github.com/1Fr3aK2)** & **[dsteiger42](https://github.com/dsteiger42)**
- Contributors visible in the [repository graph](https://github.com/1Fr3aK2/Webserver/graphs/contributors)

---

## 📄 License

This project was developed for educational purposes at **42 School**. No explicit license has been applied — please refer to the school's academic integrity policy before reusing any code.

---

> *"Every request deserves a proper response."*
