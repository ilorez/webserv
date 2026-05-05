
#include "../../includes/container.hpp"

std::string Client::readHeaders()
{
  // read to the lines before \r\n\r\n "headers" 
  char tmp[BUF_SIZE]; // 8kb
  size_t pos = 0;
  int bytes = recv(_fd, tmp, BUF_SIZE, 0);
  if (bytes <= 0)
  {
    DEBUG_WARN("error with recv in reading headers, or client discoonect");
    _state = DONE;
    return "";
  }
  this->appendToReadBuffer(tmp, bytes);
  pos = _readBuffer.find("\r\n\r\n");
  if ( pos == std::string::npos)
  {
      if (_readBuffer.size() >= MAX_HEADER_SIZE)
      {
        DEBUG_ERROR("headers is to large, more then 16kb");
        callError(431);
      }
    // stay in reading headers and move to next client
    return "";
  }
  if (pos < 1)
  {
    DEBUG_ERROR("Empty request");
    // send bad request
    callError(400);
    return "";
  }
  std::string headers = _readBuffer.substr(0, pos);
  //std::cout << headers << std::endl;
  this->setReadBuffer(_readBuffer.substr(pos+4));
  return headers;
}


void Client::preSetup()
{
  DEBUG_INFO("Client Pre Setup called");
  if (_req.getMethod() == "POST")
  {
    if (_readBuffer.size() >= _req.getContentLen())
    {
      _state = PROCESSING;
      return;
    }
    _state = READING_BODY;
    _req.setIsRequestLarge(_req.getContentLen() > USE_TMP_SIZE);
    if (_req.isRequsetLarge())
      if (!this->createTmpFile())
        _state = DONE;
  }
  else
    _state = PROCESSING;

  this->switchToEpollOut();
}

void Client::readbody()
{
  if (_req.isRequsetLarge())
    this->readFromSocket();   // write to tmp
  else
    this->readFromSocket(0); // write to string
}

void  Client::readFromSocket()
{
  /* READING_BODY:
  write chunk to client.tmp_file (random.tmp)
  client.bytes_read += n
  if bytes_read < Content-Length → stay in READING_BODY
  if bytes_read == Content-Length →
    close tmp_file
    transition to PROCESSING
  */
  // register client as EPOLLOUT using epoll_ctl(MOD)
  //std::cout << cl->getReq().getBytesCounter() << std::endl;
  char buf[CHUNK_SIZE];
  // read from socket the chunk size
  int bytes = recv(_fd, buf, CHUNK_SIZE, 0);
  if (bytes <= 0)
  {
    DEBUG_WARN("readFromSocket: error with recv in reading headers, or client discoonect");
    _state = DONE;
    return ;
  }
  _req.incrementBytesCounter(bytes);
  write(_req.getTmpFd(), buf, bytes);
  if (_req.getBytesCounter() >= _req.getContentLen())
    _state = PROCESSING;
}

void  Client::readFromSocket(int)
{
  char buf[CHUNK_SIZE];
  int bytes = recv(_fd, buf, CHUNK_SIZE, 0);
  if (bytes <= 0)
  {
    DEBUG_WARN("readFromSocket: error with recv in reading headers, or client discoonect");
    _state = DONE;
    return ;
  }
  // put in the readbuffer
  this->appendToReadBuffer(buf, bytes);
  if (_readBuffer.size() >= _req.getContentLen())
    _state = PROCESSING;
}

void Client::processing()
{
  /*EPOLLOUT fires on client_fd:
  TODO: PROCESSING:
  build response headers
  DELETE → execute delete, transition to RESPONDING
  GET    → find file, transition to RESPONDING
  POST   → move tmp_file to final location, transition to RESPONDING
  */
  DEBUG_INFO("Response");
  this->_res.setReq(this->_req);
  if (_status_error)
  {
    DEBUG_INFO("send error page");
    this->setWriteBuffer(_res.build(_status_error));
  }
  else
    this->setWriteBuffer(_res.build());
  _state = SENDING;
}


void Client::sendResponse()
{
/*
    Sending:
      DELETE/POST → response is small, write once → transition to DONE
      GET →
        read next chunk from file
        write chunk to socket
        if more chunks → stay in RESPONDING
        if file done   → transition to DONE

    DONE:
       disconnect()
    TODO: read from file and send to socket
      - build response headers and send to socket
      - check if isResponseLarge, if it read from fd chunk by chunk and send to socket
    */
    
    // send data to client and close connection after done
    send(_fd, _writeBuffer.c_str(), _writeBuffer.size(), 0);
    _state = DONE;
}


void Client::sendFromFile(int file_fd)
{
    if (_writeBuffer.empty())
    {
        char buf[CHUNK_SIZE];
        ssize_t bytes = read(file_fd, buf, CHUNK_SIZE);
        if (bytes < 0)
        {
            DEBUG_WARN("sendFromFile: error with read");
            return;
        }
        if (bytes == 0)
        {
            _state = DONE;
            return;
        }
        ssize_t bytes_sended = send(_fd, buf, bytes, 0);
        if (bytes_sended <= 0)
        {
            DEBUG_WARN("sendFromFile: error/disconnect on send");
            return;
        }
        // NOTE:
        // writeBuf holds only the unsent remainder from the last read.
        // writeOffset tracks position within that remainder on subsequent partial sends.
        this->setWriteBuffer(std::string(buf + bytes_sended, bytes - bytes_sended));
        return;
    }
    ssize_t bytes_sended = send(_fd,
       _writeBuffer.c_str() + _writeOffset,
        _writeBuffer.size() - _writeOffset, 0);
    if (bytes_sended <= 0)
    {
        DEBUG_WARN("sendFromFile: error/disconnect on send");
        return;
    }
    this->advanceWriteOffset(bytes_sended);
    if (_writeOffset == _writeBuffer.size())
        this->clearWriteBuffer();// set offset to 0
}
