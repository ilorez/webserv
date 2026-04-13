#include "../../includes/ManageClients.hpp"
#include "../../includes/container.hpp"
#include <sys/epoll.h>
#include <utility>


ManageClients::ManageClients() {}

ManageClients::~ManageClients()
{
    std::map<int, Client*>::iterator it = _clients.begin();
    while (it != _clients.end())
    {
        delete it->second;
        ++it;
    }
    _clients.clear();
}
 

// its private you can't use this 
ManageClients::ManageClients(const ManageClients &other)
{
	(void)other;
}
ManageClients &ManageClients::operator=(const ManageClients &other)
{
	(void)other;
	return (*this);
}

void    ManageClients::addClient(int fd){
  if (_clients.find(fd) != _clients.end()) {
    DEBUG_ERROR("manage client: addClient: client already exist");
    return ;
  }
  Client* cl = new Client(fd);
  _clients.insert(std::make_pair(fd, cl));
}

void    ManageClients::removeClient(int fd)
{
  std::map<int, Client*>::iterator it = _clients.find(fd);
  if ( it != _clients.end()) {
    delete it->second;
    _clients.erase(it);
    return ;
  }
  DEBUG_INFO("manage client: removeClient: client not exists");
}

Client* ManageClients::getClient(int fd)
{
  std::map<int, Client*>::iterator it = _clients.find(fd);
  if ( it != _clients.end())
    return it->second;
  return NULL;
}

bool    ManageClients::clientExists(int fd) const
{
  return _clients.find(fd) != _clients.end();
}

void ManageClients::setEpfd(int fd)
{
  _epfd = fd;
}

void ManageClients::checkTimeout()
{
  std::map<int, Client*>::iterator it = _clients.begin();
  while (it != _clients.end())
  {
    if (it->second->isTimedOut(TIMEOUT_SECONDS))
    {
      // delete from epoll
      epoll_ctl(_epfd, EPOLL_CTL_DEL, it->first, NULL);
      delete it->second;
      _clients.erase(it);
    }
    it++;
  }
}

void ManageClients::disconnect(int fd)
{
    epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
    this->removeClient(fd);
}
