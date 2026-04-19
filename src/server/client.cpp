#include "../../includes/Client.hpp"
#include "../../includes/debug.hpp"
#include <unistd.h>

Client::Client(int fd) : _fd(fd), _writeOffset(0), _lastActivity(time(NULL)),
	_state(READING_HEADERS){
}

Client::~Client()
{
  DEBUG_INFO("Client disructor called");
  close(_fd);
  
}

// its private you can't use this 
Client::Client(const Client &other)
{
	(void)other;
}
Client &Client::operator=(const Client &other)
{
	(void)other;
	return (*this);
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


// setters
void Client::setState(ClientState state)
{
  DEBUG_INFO("status change to");
  std::cout << "num: " << state << std::endl;
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

bool Client::isTimedOut(int timeoutSeconds) const
{
	return ((time(NULL) - _lastActivity) > timeoutSeconds);
}


