
#include "../../includes/container.hpp"

void Server::readheaders(Client *cl)
{
  /*
  read client headers
  n = read(fd, buf, sizeof(buf))
   user close connection n == 0  → EOF       → disconnect()
   error with socket or internet conneciton n == -1 → error     → disconnect()
   n > 0   → got data  → check client state:
  READING_HEADERS:
    append buf to client.header_buffer
    scan for "\r\n\r\n"
    if not found → stay in READING_HEADERS (wait for more data)
    if found →
      send headers to your parsing teammate
      check method:
        DELETE → transition to PROCESSING GET    → transition to PROCESSING
        POST   → transition to READING_BODY
  */
  // read from user client socket
  //
  // read to the line before \r\n\r\n 
  char tmp[BUF_SIZE]; // 8kb
  size_t pos = 0;
  //TODO: how about if user sending one byte by one byte
  int bytes = recv(cl->getFd(), tmp, BUF_SIZE, 0);
  if (bytes <= 0)
  {
    DEBUG_WARN("error with recv in reading headers, or client discoonect");
    _clients.disconnect(cl->getFd());
    return ;
  }
  cl->appendToReadBuffer(tmp, bytes);
  pos = cl->getReadBuffer().find("\r\n\r\n");
  if ( pos == std::string::npos)
  {
      if (cl->getReadBuffer().size() > MAX_HEADER_SIZE)
      {
        DEBUG_ERROR("headers is to large, more then 16kb");
        //TODO: send error 431 to client 
        // 431 Request Header Fields Too Large
        _clients.disconnect(cl->getFd());
      }
    // stay in reading headers and move to next client
    return;
  }
  if (pos < 1)
  {
    DEBUG_ERROR("Empty request");
    // TODO: send bad request
    _clients.disconnect(cl->getFd());
    return;
  }
  std::string headers = cl->getReadBuffer().substr(0, pos);
  //std::cout << headers << std::endl;
  cl->setReadBuffer(cl->getReadBuffer().substr(pos+4));
  DEBUG_INFO("Request");
  std::cout << headers << std::endl;
  try {
    req.requestParser(headers);
  } catch (const std::exception &e)
  {
    std::cerr << ERROR_MSG << "request 1: "<< e.what() << std::endl;
    // TODO: send bad request
  }
  if (req.getMethod() == "POST")
  {
    if (cl->getReadBuffer().size() >= req.getContentLen())
    {
      cl->setState(PROCESSING);
      return;
    }
    cl->setState(READING_BODY);
    req.setIsRequestLarge(req.getContentLen() > USE_TMP_SIZE);
    if (req.isRequsetLarge())
      if (!this->createTmpFile(cl))
        _clients.disconnect(cl->getFd());
  }
  else
    cl->setState(PROCESSING);
}

void Server::readrequest(Client *cl)
{
  if (cl->getState() == READING_HEADERS)
    this->readheaders(cl);
  if (cl->getState() == READING_BODY) 
  {
    if (req.isRequsetLarge())
      readFromSocket(cl);   // write to tmp
    else
      readFromSocket(cl, 0); // write to string
  }
  if (
      cl->getState() == PROCESSING 
      || cl->getState() == SENDING 
      || cl->getState() == DONE
      )
  {
  _switchEpollRegisration(cl->getFd(),EPOLLOUT);
  epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, cl->getFd(), &_epoll_event);
  }
}

void  Server::readFromSocket(Client *cl)
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
  //std::cout << req.getBytesCounter() << std::endl;
  char buf[CHUNK_SIZE];
  // read from socket the chunk size
  int bytes = recv(cl->getFd(), buf, CHUNK_SIZE, 0);
  if (bytes <= 0)
  {
    DEBUG_WARN("readFromSocket: error with recv in reading headers, or client discoonect");
    _clients.disconnect(cl->getFd());
    return ;
  }
  req.incrementBytesCounter(bytes);
  write(req.getTmpFd(), buf, bytes);
  if (req.getBytesCounter() >= req.getContentLen())
    cl->setState(PROCESSING);
}

void  Server::readFromSocket(Client *cl, int)
{
  char buf[CHUNK_SIZE];
  int bytes = recv(cl->getFd(), buf, CHUNK_SIZE, 0);
  if (bytes <= 0)
  {
    DEBUG_WARN("readFromSocket: error with recv in reading headers, or client discoonect");
    _clients.disconnect(cl->getFd());
    return ;
  }
  // put in the readbuffer
  cl->appendToReadBuffer(buf, bytes);
  if (cl->getReadBuffer().size() >= req.getContentLen())
    cl->setState(PROCESSING);
}
