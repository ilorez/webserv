/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 22:04:59 by znajdaou          #+#    #+#             */
/*   Updated: 2026/05/19 22:05:00 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/container.hpp"
#include <algorithm>
#include <sys/epoll.h>

// #constructors
Server::Server(std::vector<ServerConfig> &servers):_epoll_fd(-1), _servers(servers), _is_running(true)
{
  _epoll_event.events = EPOLLIN;
  _epoll_event.data.ptr = NULL;
}

Server::~Server() 
{
  DEBUG_INFO("SERVER destructor called");
  if (_epoll_fd > -1)
    close(_epoll_fd);
  for (unsigned long i = 0; i < _servers.size(); i++)
  {
    if (_servers[i].getFd() > -1)
      close (_servers[i].getFd());
  }
}

Server::Server(const Server &o):_servers(o._servers) {
  *this = o;
}

Server& Server::operator=(const Server &o) {
  (void) o;
  return *this;
}

void Server::run() 
{
  _epoll_fd = epoll_create(1);
  if (_epoll_fd == -1)
    throw ServerException("server run: epoll_create fail");
  // while on vector size
  for (unsigned long i = 0; i < _servers.size(); i++)
    this->_initSocket(_servers[i]); 
  _clients.setEpfd(_epoll_fd);
  while (_is_running)
    this->_handelClient();
}

void Server::_handelClient()
{
  DEBUG_INFO("------------------");
  int n = epoll_wait(_epoll_fd, _events, MAX_EVENTS, EPOLL_WAIT_TIMEOUT);
  if (n == -1)
  {
    if (errno == EINTR)
    {
        _is_running = false;
        return;
    }
    throw ServerException("epoll_wait: failed");
  }
  for (int i = 0; i < n; i++)
  {
    t_epollhold *eh = static_cast<t_epollhold*>(_events[i].data.ptr);
    if (!eh)
    {
        DEBUG_ERROR("epoll data ptr is invalid");
        continue;
    }
    //DEBUG_INFO("event fired on fd: " + to_string98(eh->fd));
  bool is_new_conx = false;
  for (unsigned long i = 0; i < _servers.size(); i++)
  {
    if (eh->fd == _servers[i].getFd())
    {
      this->newconnection(_servers[i]);
      is_new_conx = true;
      break;
    }
  }
  if (is_new_conx){continue;}
  else if (_events[i].events & EPOLLIN || _events[i].events & EPOLLOUT)
  {
    if (eh->cl->isTimedOut(TIMEOUT_SECONDS))
      continue;
    else if (eh->is_cgi)
      eh->cgi->handel(eh->fd, _events[i].events);
    else if (eh->cl->getState() == READING_HEADERS)
      this->readheaders(eh->cl);
    else
      eh->cl->handel(0, _events[i].events);
    if (eh->cl->getState() ==  DONE)
      eh->cl->forceTimeout();
    else
      eh->cl->updateLastActivity();
  }
  else if (_events[i].events & ( EPOLLHUP | EPOLLERR | EPOLLRDHUP))
    eh->cl->forceTimeout();
  else 
    DEBUG_ERROR("Unknown event firedon");
  }
  _clients.checkTimeout();
}

