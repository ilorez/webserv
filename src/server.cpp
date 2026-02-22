
#include "../includes/Server.hpp"
#include "../includes/container.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>


Server::Server()
{
  _port = 8080;
  _ip = "127.0.0.1";
}

Server::~Server()
{
  close(_socket_fd);
}

void Server::run()
{
  this->_initSocket();
  socklen_t size_socket = sizeof(_addr);
  while(1)
    this->_handelClient(size_socket);
}

void Server::_initSocket()
{
  _socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (_socket_fd < 0)
  {
    throw ServerException("socket() failed.");
  }
  int opt = 1;
  setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  memset(&_addr, 0, sizeof(_addr));
  _addr.sin_family = AF_INET;
  _addr.sin_port = htons(_port);
  _addr.sin_addr.s_addr = inet_addr(_ip.c_str());
  if (bind(_socket_fd, (struct sockaddr*)(&_addr), sizeof(_addr)))
  {
    throw ServerException("bind() failed with port" + to_string98(_port));
  }

  if (listen(_socket_fd, MAX_CONX_QUEUE))
    throw ServerException("listen() failed.");
  std::cout << INFO_MSG << "listen on http:://" << inet_ntoa(_addr.sin_addr) <<  ":" <<  to_string98(ntohs(_addr.sin_port)) << std::endl;
}

void Server::_handelClient(socklen_t size_socket)
{
  int client_fd = accept(_socket_fd, (struct sockaddr*)(&_addr), (socklen_t *)&size_socket);
  if (client_fd  < 0)
    throw ServerException("accept() failed.");
  char buf[BUF_SIZE]; 
  memset(&buf, 0, BUF_SIZE);
  // read from user fd
  size_t readed = read(client_fd, buf, BUF_SIZE);
  std::cout << "Request content" << std::endl;
  std::cout << "size: " << readed << std::endl;
  std::cout << buf << std::endl;
  std::cout << "------ Request end -------" << std::endl;
  Request req;
  req.request_pars(buf);
  std::string page_src;
  if (req.getRoute() != "/")
    page_src = "./www/404.html";
  else
    page_src = "./www/index.html";
  std::string body = ft_readFile(page_src);
  std::string response = "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/html\r\n"
                     "Content-Length: " + to_string98(body.length()) + "\r\n"
                     "\r\n"// this how reader know the end of header
                     + body;
  // write back to user fd
  write(client_fd, response.c_str(), response.length());
  // close 
  close(client_fd);
}
