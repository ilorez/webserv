#include "../../includes/ManageClients.hpp"
#include "../../includes/container.hpp"
#include <sys/epoll.h>
#include <utility>


ManageClients::ManageClients() {}

ManageClients::~ManageClients()
{
    DEBUG_INFO("ManageClients destructor called");
    this->disconnectAll();
}

void    ManageClients::disconnectAll()
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

Client*    ManageClients::addClient(int fd){
  std::map<int, Client*>::iterator it = _clients.find(fd);
  if ( it != _clients.end()) {
    DEBUG_ERROR("manage client: addClient: client already exist");
    return it->second; // TODO: is it really fine like that
  }
  Client* cl = new Client(fd, _epfd);
  _clients.insert(std::make_pair(fd, cl));
  return cl;
}

void    ManageClients::disconnect(int fd)
{
  std::map<int, Client*>::iterator it = _clients.find(fd);
  if ( it != _clients.end()) {
    delete it->second;
    _clients.erase(it);
    return ;
  }
  DEBUG_ERROR("manage client: removeClient: client not exists");
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
      // closing fd removes the fd from epoll
      delete it->second;
      _clients.erase(it++);
    }
    else
      ++it;
  }
}

CGIClient* ManageClients::updateToCGI(int fd) { 
  //DEBUG_INFO("update to CGI called");
  Client *oc = _clients[fd];
  CGIClient* nc = new CGIClient(*_clients[fd]); 
  _clients[fd] = nc;
  delete oc; 
  return nc;
}
