
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
  int bytes = read(_pipe_out[0], buf, CHUNK_SIZE);
  if (bytes == -1)
    throw CGIException("read: readToWriteBuffer: failed");
  // EPOLLIN will be fired everytime if its found that the pipe has been closed
  if (bytes == 0)
  {
    // TODO last
    // done
    // unregister pipe out
    _cgi_pipe_done = true;
  }


}

