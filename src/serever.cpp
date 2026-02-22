
#include "../includes/Server.hpp"
#include "../includes/container.hpp"
#include <arpa/inet.h>


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
  {
    throw ServerException("listen() failed.");
  }
  DEBUG_INFO("listen on http:://" + inet_ntoa(_addr.sin_addr) + ":" + to_string98(ntohs(_addr.sin_port)));
}


