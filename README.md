# Simple HTTP Server in C++

A minimal HTTP server written in C++98 that listens on a port and returns a Hello World HTML page.

---

## Requirements

- `g++` or `clang++`
- `make`
- Linux or macOS

---

## Build

```bash
make
```

To rebuild from scratch:

```bash
make re
```

To clean build files:

```bash
make clean
```

---

## Run

```bash
./webserv
```

The server will start and print:

```
Listening on: http://127.0.0.1:8080
```

---

## Test

**In your browser:**

```
http://localhost:8080
```

**With telnet (raw TCP):**

```bash
telnet localhost 8080
```

Then type:

```
GET / HTTP/1.1
Host: localhost
```

Press `Enter` twice. You will see the raw HTTP response.

**With curl:**

```bash
curl http://localhost:8080
```

---

## Project Structure

```
.
├── Makefile
├── README.md
├── main.cpp
├── includes/
│   ├── container.hpp
│   ├── Server.hpp
│   ├── Request.hpp
│   ├── Response.hpp
│   └── WebServExceptions.hpp
└── src/
    ├── server.cpp
    ├── request.cpp
    ├── response.cpp
    └── utils.cpp
```

---

## How it works

```
socket()       create a TCP socket
setsockopt()   allow port reuse (SO_REUSEADDR)
bind()         attach socket to 127.0.0.1:8080
listen()       wait for incoming connections
accept()       accept a client connection
read()         read the HTTP request
Request        parse method, path, headers
Response       build a valid HTTP response
write()        send the response back
close()        close the client connection
```

---

## HTTP Response Example

Every request receives:

```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 20

<h1>Hello World</h1>
```

---

## Stop the server

```bash
Ctrl + C
```

---

## Notes

- Compiled with `-std=c++98`
- No external libraries used
- Single threaded — handles one client at a time
- Only listens on `127.0.0.1` (localhost), not exposed to the network
