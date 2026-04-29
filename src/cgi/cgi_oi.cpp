
#include "../../includes/container.hpp"

void CGIClient::writeToReadBuffer()
{
  char buf[CHUNK_SIZE];
  int bytes = recv(this->_fd, buf, CHUNK_SIZE, 0);
  if (bytes <= 0)
    throw CGIException("recv: readToReadBuffer: failed, client disconnected");
  // put in the readbuffer
  this->appendToReadBuffer(buf, bytes);
  _read_counter += bytes;
  // unregister socket
  // register pipe in 1
  if (_read_counter >= _req.getContentLen())
    _socket_done = true;
}


void CGIClient::writeToPipe()
{
  int bytes = write(_pipe_in[1], _readBuffer.c_str(), _readBuffer.size());
  if (bytes == -1)
    throw CGIException("write: writeToPipe: failed");
  if (bytes < _readBuffer.size())
  {
    // keep pipe registred
    // return
  }
  // unregister pipe
  if (!_socket_done)
  {}// register socket again with EPOLLIN
}

void CGIClient::writeToWriteBuffer()
{
  char buf[CHUNK_SIZE];
  _writeOffset = 0;
  int bytes = read(_pipe_out[0], buf, CHUNK_SIZE);
  if (bytes == -1)
    throw CGIException("read: readToWriteBuffer: failed");
  // EPOLLIN will be fired everytime if its found that the pipe has been closed
  // unregister pipe out
  if (bytes == 0)
  {
    // done
    _cgi_pipe_done = true;
    return;
  }
  this->setWriteBuffer(std::string(buf, bytes));
  // register socket EPOLLOUT 
}

void CGIClient::writeToSocket()
{
    ssize_t bytes_sended = send(_fd,
    _writeBuffer.c_str() + _writeOffset,
    _writeBuffer.size() - _writeOffset, 0);
    if (bytes_sended <= 0)
        throw CGIException("send: writeToScocket: cgi: failed, client disconnected");
    advanceWriteOffset(bytes_sended);
    if (_writeOffset == _writeBuffer.size())
    {
        clearWriteBuffer();// set offset to 0
        // unregister socket out
        if (_cgi_pipe_done)
          this->setState(DONE);
        else {}
          // register pipe out 0
    }
}


void ep_unregister() {
// TODO: last here
// how i can manage the epollholder pointers so i can free theme any time ??

}

          epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, eh->cl->getFd(), &_epoll_event);}
