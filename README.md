*This project has been created as part of the 42 curriculum by znajdaou, aezghari, mohalaou.*

---

# Webserv — HTTP/1.0 Web Server in C++98

## Description

Webserv is a fully functional HTTP/1.0 web server written entirely in C++98, built from the ground up without any external networking libraries or frameworks. The goal of this project is to gain a deep, hands-on understanding of how web servers work at every layer — from raw TCP socket management to HTTP protocol semantics and CGI script execution.

Every time a client sends a request, the server:
1. Receives raw bytes over a TCP socket
2. Parses the HTTP request line, headers, and body from scratch
3. Applies routing rules from an Nginx-style configuration file
4. Executes external CGI scripts asynchronously if needed
5. Constructs and delivers a fully compliant HTTP/1.0 response

### Key Features

- **Non-blocking I/O multiplexing** with `epoll()` — a single thread manages up to 900 simultaneous connections
- **Virtual hosting** — multiple named servers sharing a single port, selected by the `Host:` header
- **HTTP methods**: GET, POST, DELETE
- **Asynchronous CGI execution** via `fork()` + `execve()` + dual non-blocking pipes
- **Cookie-based session management** stored in-memory
- **Custom error pages** with a template substitution system

## Instructions

### Prerequisites

- A C++ compiler with C++98 support (`c++`)
- A POSIX-compatible OS (Linux or macOS)
- `make`

### Compilation

```bash
git clone <intra_git_repository-url> webserv
cd webserv
make
```

This produces the `webserv` executable, compiled with strict flags:

```
-Wall -Wextra -Werror -std=c++98
```

Additional Makefile targets:

```bash
make clean    # Remove compiled object files
make fclean   # Remove object files and the binary
make re       # Full clean rebuild
```

### Running the Server

```bash

# Run with a specific configuration file
./webserv conf/default[Number].conf

# Run with a custom configuration
./webserv path/to_your_custom_config_file.conf
```

The server listens on the ports defined in the configuration file. Stop it at any time with `Ctrl+C` — the server performs a clean shutdown, closing all open file descriptors.

### Testing

```
The server ships with three built-in web UIs to test its features interactively in the browser.

## Media Tester — `http://localhost:8080/`

Tests file upload (`POST`), retrieval (`GET`), and deletion (`DELETE`) against the `/upload/` route.
After uploading a file, the `GET` panel fetches it back and renders a live preview directly in the browser — images, videos, audio, and text files are all previewed inline.

---

## CGI Tester — `http://127.0.0.1:4444/cgi/`

Tests the full CGI lifecycle.
You can upload a `.py` script to the server, run it via `GET` (no body) or `POST` (with a custom text body or a file), and inspect the raw output.
You can also delete scripts from the same page.

---

## Cookie Tester — `http://127.0.0.1:4444/cookies/`

Tests cookie-based session persistence.
The page lets you save a theme cookie (`dark/light`) and a language cookie, view all currently stored cookies, and delete them.
The theme is applied immediately to the page — on reload, the server reads the cookie and restores your preferences automatically.
```

### Configuration

The configuration file uses an Nginx-inspired syntax. Example:

```nginx
server {
    listen 8080;
    host 127.0.0.1;
    server_name my_server;

    root ./www;
    index index.html;

    client_max_body_size 10M;
    error_page 404 /error.html;

    location / {
        allow_methods GET;
    }

    location /upload {
        allow_methods GET POST;
        upload_store ./uploads;
        autoindex on;
    }

    location /cgi-bin {
        allow_methods GET POST;
        cgi_handler .py /usr/bin/python3;
    }
}
```

Key directives: `listen`, `server_name`, `client_max_body_size`, `error_page`, `root`, `index`, `autoindex`, `allowed_methods`, `cgi_extension`, `cgi_path`, `upload_enable`, `upload_store`, `return` (redirect).

---

## Resources

### Official Specifications

| Resource | URL |
|----------|-----|
| RFC 1945 — HTTP/1.0 | https://www.rfc-editor.org/info/rfc1945/ |
| RFC 3875 — CGI/1.1  | https://www.rfc-editor.org/info/rfc3875/ |

### Books

| Resource | URL |
|----------|-----|
| HTTP: The Definitive Guide — David Gourley & Brian Totty (O'Reilly) | https://tools.ietf.org/html/rfc7230 |

### Network Programming

| Resource | URL |
|----------|-----|
| Beej's Guide to Network Programming | https://github.com/oxidation99/MyBooks-1/blob/master/HTTP%20The%20%20Definitive%20Guide.pdf |

### Linux Man Pages

| Syscall | URL |
|---------|-----|
| `epoll(2)` | https://man7.org/linux/man-pages/man7/epoll.7.html |
| `socket(2)` | https://man7.org/linux/man-pages/man2/socket.2.html |
| `fcntl(2)` | https://man7.org/linux/man-pages/man2/fcntl.2.html |
| `fork(2)` | https://man7.org/linux/man-pages/man2/fork.2.html |
| `execve(2)` | https://man7.org/linux/man-pages/man2/execve.2.html |
| `pipe(2)` | https://man7.org/linux/man-pages/man2/pipe.2.html |

### AI Usage Disclosure

Artificial Intelligence was used during the development of this project as a conceptual guide and pair-programming assistant. Specifically, AI assisted with:

- **Frontend development** — Guiding the design and implementation of the HTML/CSS web interface, login pages, and test sites hosted by the server.
- **Providing information** — Answering technical questions about HTTP protocol behavior, socket programming, and CGI specification details during development.
- **Code readability & structure** — Reviewing and improving the organization, naming conventions, and structure of specific parts of the codebase to make them cleaner and easier to navigate.
- **Documentation** — Structuring this README.