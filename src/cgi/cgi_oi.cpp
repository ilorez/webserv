
#include "../../includes/container.hpp"
#include <algorithm>
#include <sys/epoll.h>
#include <sys/types.h>

void CGIClient::writeToReadBuffer()
{
  DEBUG_INFO("writeToReadBuffer called");
  char buf[CHUNK_SIZE];
  int bytes = recv(this->_fd, buf, CHUNK_SIZE, 0);
  std::cout << bytes << std::endl;
  if (bytes <= 0)
    throw CGIException("recv: reading from socket failed!");
  // put in the readbuffer
  _read_counter += bytes;
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
  //std::cout << "readbuffer size: "<< _read_counter << std::endl;
  //std::cout << "content len: "<< _req.getContentLen() << std::endl;
  if (_read_counter >= _req.getContentLen())
    _socket_done = true;
  //if (_socket_done)
   // DEBUG_INFO2("CONTENTLEN ritched");
}

void CGIClient::writeToPipe()
{
  DEBUG_INFO("writeToPipe called");

  //std::cout << _readBuffer << std::endl;
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
  else
    ft_closefd(_pipe_in[1]);
  //DEBUG_INFO("writeToPipe finish");
}

void CGIClient::writeToWriteBuffer()
{
  DEBUG_INFO("writeToWriteBuffer called");
  char buf[CHUNK_SIZE];

  // TODO: why i add this to here
  //_writeOffset = 0;
  int bytes = read(_pipe_out[0], buf, CHUNK_SIZE);
  //DEBUG_INFO("bytes read from pipe: " + to_string98(bytes));
  if (bytes == -1)
    throw CGIException("read: readToWriteBuffer: failed");
  // EPOLLIN will be fired everytime if its found that the pipe has been closed
  // unregister pipe out 0
  DEBUG_INFO2("1");
  if (bytes == 0)
  {
    // done
    _cgi_pipe_done = true;
    epoll_ctl(_epfd, EPOLL_CTL_DEL, _pipe_out[0], NULL);
    return;
  }
  std::string chunk(buf, bytes);
  _writeBuffer +=  chunk;
  std::cout << _writeBuffer << std::endl;
  if (!_got_headers_end)
  {
      size_t pos = chunk.find("\r\n\r\n");
      if (pos == std::string::npos)
      {
          pos = chunk.find("\n\n");
          if (pos == std::string::npos)
            return;
          replace_all(_writeBuffer, "\r\n", "\n");
      }
      _got_headers_end = true;
      std::string status = "200 OK";
      size_t status_pos = _writeBuffer.find("Status:");
      if (status_pos != std::string::npos && status_pos < pos)
      {
          size_t end = _writeBuffer.find("\n", status_pos);
          status = _writeBuffer.substr(status_pos + 7, end - (status_pos + 7));
  
          size_t start = status.find_first_not_of(" \t");
          if (start != std::string::npos)
              status = status.substr(start);
      }
  
      size_t loc_pos = _writeBuffer.find("Location:");
      if (loc_pos != std::string::npos && loc_pos < pos)
      {
          size_t loc_end = _writeBuffer.find("\n", loc_pos);
          std::string loc_val = _writeBuffer.substr(loc_pos + 9,
                                                    loc_end - (loc_pos + 9));
  
          size_t start = loc_val.find_first_not_of(" \t");
          if (start != std::string::npos)
              loc_val = loc_val.substr(start);
  
          if (!loc_val.empty() && status == "200 OK")
              status = "302 Found";
      }
  
      _writeBuffer =
          "HTTP/1.0 " + status + "\r\n" +
          _writeBuffer;
  }

  DEBUG_INFO2("4");
  epoll_ctl(_epfd, EPOLL_CTL_DEL, _pipe_out[0], NULL);
  //DEBUG_INFO2("writeBuffer: ");
  //std::cout << _writeBuffer << std::endl;
  // register socket EPOLLOUT 
  _epoll_events =  _epoll_events | EPOLLOUT;
  struct epoll_event ev = create_ev(&_clsock_hold, _epoll_events);
  epoll_ctl(_epfd, EPOLL_CTL_MOD, _fd, &ev);
  //epoll_ctl(_epfd, EPOLL_CTL_ADD, _fd, &ev);
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

