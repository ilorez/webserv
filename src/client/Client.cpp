#include "../../includes/container.hpp"
#include <iostream>
#include <sys/epoll.h>

// Constructor
Client::Client(int fd, int epfd)
    : _fd(fd),
      _epfd(epfd),
      _epoll_events(EPOLLIN | EPOLLRDHUP),
      _readBuffer(""),
      _writeBuffer(""),
      _writeOffset(0),
      _lastActivity(time(NULL)),
      _state(READING_HEADERS),
      _req(),
      _clsock_hold(),
      _status_error(0),
      _res()
{
    _clsock_hold.fd = fd;
    _clsock_hold.is_cgi = false;
    _clsock_hold.cl = this;
}

Client::Client(const Client& other)
    : _fd(other._fd),
      _epfd(other._epfd),
      _epoll_events(other._epoll_events),
      _readBuffer(other._readBuffer),
      _writeBuffer(other._writeBuffer),
      _writeOffset(other._writeOffset),
      _lastActivity(other._lastActivity),
      _state(other._state),
      _req(other._req),
      _clsock_hold(other._clsock_hold),
      _status_error(other._status_error),
      _res(other._res)
{
    *this = other; // still fine, self-assignment guard handles it
}

Client& Client::operator=(const Client& other)
{
    if (this == &other)
        return *this;
    _fd           = other._fd;
    _epfd         = other._epfd;
    _epoll_events = other._epoll_events;
    _readBuffer   = other._readBuffer;
    _writeBuffer  = other._writeBuffer;
    _writeOffset  = other._writeOffset;
    _lastActivity = other._lastActivity;
    _state        = other._state;
    _req          = other._req;
    _clsock_hold  = other._clsock_hold; // covers .fd, .is_cgi, .cl
    _status_error = other._status_error;
    _res          = other._res;
    return *this;
}

Client::~Client()
{
  DEBUG_INFO("Client disructor called");
  if (_fd >= 0)
    close(_fd);
}

// getters
int Client::getFd() const
{
	return (_fd);
}
const std::string &Client::getReadBuffer() const
{
	return (_readBuffer);
}
const std::string &Client::getWriteBuffer() const
{
	return (_writeBuffer);
}
size_t Client::getWriteOffset() const
{
	return (_writeOffset);
}
time_t Client::getLastActivity() const
{
	return (_lastActivity);
}
ClientState Client::getState() const
{
  	return (_state);
}

Request& Client::getReq()
{
  return _req;
}

t_epollhold& Client::getClSockHolder() 
{
  return _clsock_hold;
}


// setters
void Client::setState(ClientState state)
{
  //DEBUG_INFO("status change to");
  //std::cout << "num: " << state << std::endl;
	_state = state;
}

void Client::setWriteBuffer(const std::string &data)
{
	_writeBuffer = data;
}

void Client::setReadBuffer(const std::string &data)
{
	_readBuffer = data;
}

void Client::updateLastActivity()
{
	_lastActivity = time(NULL);
}

void Client::invalidateFd()
{
  _fd = -1;
}

// methods
void Client::appendToReadBuffer(const char *data, size_t len)
{
	_readBuffer.append(data, len);
}

void Client::advanceWriteOffset(size_t bytes)
{
	_writeOffset += bytes;
}

void Client::clearReadBuffer()
{
	_readBuffer.clear();
}

void Client::clearWriteBuffer()
{
	_writeBuffer.clear();
	_writeOffset = 0;
}

void  Client::forceTimeout()
{
  DEBUG_INFO("force timeout called");
  time_t epoch = 0;
  _lastActivity = epoch;
}


bool Client::isTimedOut(int timeoutSeconds) const
{
	return ((time(NULL) - _lastActivity) > timeoutSeconds);
}

void Client::callError(int err_code)
{
    _status_error = err_code;
    _state = PROCESSING;
}

bool Client::createTmpFile()
{
  _req.setTmpFileName(makeTmpPath(_fd));
  int tfd = open(_req.getTmpFileName().c_str(), O_RDWR | O_APPEND | O_CREAT, 0755);
  if (tfd < 0)
  {
    DEBUG_ERROR("readFromSocket: could not create tmp file");
    return false;
  }
  _req.setTmpFd(tfd);
  write(_req.getTmpFd(), _readBuffer.c_str(), _readBuffer.size());
  _req.setBytesCounter(_readBuffer.size());
  this->clearReadBuffer();
  return true;
}

void Client::switchToEpollOut()
{
  if (_state == PROCESSING || _state == SENDING ){
    struct epoll_event ev = create_ev(&_clsock_hold, EPOLLOUT);
    epoll_ctl(_epfd, EPOLL_CTL_MOD, _fd, &ev);
  }
}

void Client::handel(int, uint32_t evs)
{
  //DEBUG_INFO("handel client request called");
  if (evs & EPOLLIN)
  {
    if (_state != READING_BODY)
    {
      DEBUG_ERROR("EPOLLIN fired on client when whith incorrect state");
      this->setState(DONE);
      return;
    }
    this->readbody();
    this->switchToEpollOut();
  }
  else
  {
    if (_state == PROCESSING)
      this->processing();
    else if(_state == SENDING)
      this->sendResponse();
    else
    {
      DEBUG_ERROR("EPOLLOUT fired on client when whith incorrect state");
      this->setState(DONE);
      return;
    }
  }
}

