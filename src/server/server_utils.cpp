
#include "../../includes/container.hpp"
#include <exception>

// add client
void Server::_addClient(int client_fd) {
  // applying non-blocking mode to everyclient fd
  fcntl(client_fd, F_SETFL, O_NONBLOCK);
  
  // adding to epoll queu
  _switchEpollRegisration(client_fd, EPOLLIN);
  epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &_epoll_event); 
  // NOTE:epoll ctl copy the ev into kernel
}

// switch epoll regitration
void Server::_switchEpollRegisration(int client_fd, uint32_t ev)
{
  _epoll_event.data.fd = client_fd;
  _epoll_event.events = ev;
}

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
        DELETE → transition to PROCESSING
        GET    → transition to PROCESSING
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
    DEBUG_ERROR("error with recv in reading headers, or client discoonect");
    _clients.disconnect(cl->getFd());
    return ;
  }
  cl->appendToReadBuffer(tmp, bytes);
  pos = cl->getReadBuffer().find("\r\n\r\n");
  if ( pos == std::string::npos)
  {
      if (cl->getReadBuffer().size() > MAX_HEADER_SIZE)
      {
        DEBUG_ERROR("headers is to lage, more then 16kb");
        //TODO: send error 431 to client 
        // 431 Request Header Fields Too Large
        _clients.disconnect(cl->getFd());
        return ;
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
    cl->setState(READING_BODY);
  else
    cl->setState(PROCESSING);
}

void Server::newconnection(socklen_t size_socket)
{
  int client_fd = accept(_socket_fd, (struct sockaddr *)(&_addr), (socklen_t *)&size_socket);
  DEBUG_INFO("------------New Request-----------");
  if (client_fd < 0)
    throw ServerException("accept() failed.");
  // adding to clinet list
  _clients.addClient(client_fd);
  this->_addClient(client_fd);
}

void Server::readrequest(Client *cl)
{
  if (cl->getState() == READING_HEADERS)
    this->readheaders(cl);
  if (cl->getState() == READING_BODY) 
  {
    if (req.getContentLen() > USE_TMP_SIZE)
    {
      // TODO: store on tmp file
    }
    else 
    {
      // TODO: store on string
    }
    /* READING_BODY:
    write chunk to client.tmp_file (random.tmp)
    client.bytes_read += n
    if bytes_read < Content-Length → stay in READING_BODY
    if bytes_read == Content-Length →
      close tmp_file
      transition to PROCESSING
    */
    // register client as EPOLLOUT using epoll_ctl(MOD)
  }
  _switchEpollRegisration(cl->getFd(),EPOLLOUT);
  epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, cl->getFd(), &_epoll_event);
}

void Server::sendresponse(Client *cl)
{
  if (cl->getState() == PROCESSING)
  {
    /*EPOLLOUT fires on client_fd:
    TODO: PROCESSING:
    build response headers
    DELETE → execute delete, transition to RESPONDING
    GET    → find file, transition to RESPONDING
    POST   → move tmp_file to final location, transition to RESPONDING
    
    RESPONDING:
      DELETE/POST → response is small, write once → transition to DONE
      GET →
        read next chunk from file
        write chunk to socket
        if more chunks → stay in RESPONDING
        if file done   → transition to DONE

    DONE:
       disconnect()
    */
    DEBUG_INFO("Response");
    Response res(req);
    cl->setWriteBuffer(res.build());
    cl->setState(SENDING);
  }
  else if (cl->getState() == SENDING)
  {
    DEBUG_INFO("SENDING response");
    std::cout << cl->getWriteBuffer() << std::endl;
    // send data to client and close connection after done
    send(cl->getFd(), cl->getWriteBuffer().c_str(), cl->getWriteBuffer().length(), 0);
    cl->setState(DONE); 
  }
}
