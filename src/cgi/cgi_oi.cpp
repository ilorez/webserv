
#include "../../includes/container.hpp"
#include <sys/epoll.h>
#include <sys/types.h>

void CGIClient::writeToReadBuffer()
{
  DEBUG_INFO("writeToReadBuffer called");
  char buf[CHUNK_SIZE];
  int bytes = recv(this->_fd, buf, CHUNK_SIZE, 0);
  if (bytes <= 0)
    throw CGIException("recv: readToReadBuffer: failed, client disconnected");
  // put in the readbuffer
  this->appendToReadBuffer(buf, bytes);
  this->turnToPipe();
}

void CGIClient::removeEpollinEventFromSocket()
{
  DEBUG_INFO("removeEpollinEventFromSocket called");  
  // unregister EPOLLIN event from socket 
  _epoll_events = _epoll_events & ~EPOLLIN;
  _epoll_events = _epoll_events | EPOLLHUP;
  struct epoll_event ev = create_ev(&_clsock_hold, _epoll_events);
  epoll_ctl(_epfd, EPOLL_CTL_MOD, _fd, &ev);
}

// register pipe in EPOLLOUT and unregistre socket EPOLLIN
void CGIClient::turnToPipe()
{
  DEBUG_INFO("turnToPipe called");

  // unregister EPOLLIN event from socket 
  removeEpollinEventFromSocket();

  // register pipe in 1
  struct epoll_event ev = create_ev(&_pipe_in_hold, EPOLLOUT);
  epoll_ctl(_epfd, EPOLL_CTL_ADD, _pipe_in[1], &ev);
  if (_readBuffer.size() >= _req.getContentLen())
    _socket_done = true;
}


void CGIClient::writeToPipe()
{
  DEBUG_INFO("writeToPipe called");

  ssize_t bytes = write(_pipe_in[1], _readBuffer.c_str(), _readBuffer.size());
  if (bytes == -1)
    throw CGIException("write: writeToPipe: failed");
  if (static_cast<size_t>(bytes) < _readBuffer.size())
  {
    // keep pipe registred
    // substring readBuffer
    _readBuffer = _readBuffer.substr(bytes);
    return;
  }
  // unregister pipe
  epoll_ctl(_epfd, EPOLL_CTL_DEL, _pipe_in[1], NULL);
  // clear readBuffer
  this->clearReadBuffer();
  if (!_socket_done)
  {
    // register socket again with EPOLLIN
    _epoll_events |= EPOLLIN;
   struct epoll_event ev = create_ev(&_clsock_hold, _epoll_events);
  epoll_ctl(_epfd, EPOLL_CTL_MOD, _fd, &ev);
  }

}

void CGIClient::writeToWriteBuffer()
{
  DEBUG_INFO("writeToWriteBuffer called");
  char buf[CHUNK_SIZE];

  // TODO: why i add this to here
  //_writeOffset = 0;
  int bytes = read(_pipe_out[0], buf, CHUNK_SIZE);
  DEBUG_INFO("bytes read from pipe: " + to_string98(bytes));
  if (bytes == -1)
    throw CGIException("read: readToWriteBuffer: failed");
  // EPOLLIN will be fired everytime if its found that the pipe has been closed
  // unregister pipe out 0
  epoll_ctl(_epfd, EPOLL_CTL_DEL, _pipe_out[0], NULL);
  if (bytes == 0)
  {
    // done
    _cgi_pipe_done = true;
    return;
  }
  this->setWriteBuffer(std::string(buf, bytes));
  // register socket EPOLLOUT 
  _epoll_events =  _epoll_events | EPOLLOUT;
  struct epoll_event ev = create_ev(&_clsock_hold, _epoll_events);
  epoll_ctl(_epfd, EPOLL_CTL_MOD, _fd, &ev);
  //epoll_ctl(_epfd, EPOLL_CTL_ADD, _fd, &ev);
  DEBUG_INFO("writeToWriteBuffer completed");
  // write buffer is
  std::cout << "write buffer: " << getWriteBuffer() << std::endl;
}
void CGIClient::writeToSocket()
{
    DEBUG_INFO("writeToSocket called");
    ssize_t bytes_sended = send(_fd,
    _writeBuffer.c_str() + _writeOffset,
    _writeBuffer.size() - _writeOffset, 0);
    if (bytes_sended <= 0)
        throw CGIException("send: writeToSocket: cgi: failed, client disconnected");
    advanceWriteOffset(bytes_sended);
    if (_writeOffset >= _writeBuffer.size())
    {
        clearWriteBuffer();// set offset to 0
        // unregister socket out
        _epoll_events = _epoll_events & ~EPOLLOUT;
        struct epoll_event ev = create_ev(&_clsock_hold, _epoll_events);
        epoll_ctl(_epfd, EPOLL_CTL_MOD, _fd, &ev);

        if (_cgi_pipe_done)
          this->setState(DONE);
        else {
            // register pipe out 0
            ev = create_ev(&_pipe_out_hold, EPOLLIN);
            epoll_ctl(_epfd, EPOLL_CTL_ADD, _pipe_out[0], &ev);
        }
    }
}

