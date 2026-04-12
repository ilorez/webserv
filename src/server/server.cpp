
#include "../../includes/container.hpp"

Server::Server()
{
  _port = 8080;
  _ip = "127.0.0.1";
  _epoll_event.events = EPOLLIN;
}

Server::~Server()
{
  close(_socket_fd);
}

void Server::run()
{
  this->_initSocket(); socklen_t size_socket = sizeof(_addr);
  _epoll_fd = epoll_create(1);
  if (_epoll_fd == -1)
    throw ServerException("server run: epoll_create fail");
  _clients.setEpfd(_epoll_fd);
  this->_addClient(_socket_fd);
  while (1)
    this->_handelClient(size_socket);
}

void Server::_initSocket()
{
  // AF_INET: address family ipv4
  // SOCK_STREAM: socket type: stream-based (as opposed to SOCK_DGRAM for UDP)
  // IPPROTO_TCP protocol: TCP 
  _socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (_socket_fd < 0)
    throw ServerException("socket() failed.");
  // apply non-bloacking mode to listening socket fd
  int opt = 1;
  // SOL_SOCKET: the "level" you're setting an option at the socket layer
  // SO_REUSEADDR: for reusing a local address that still in TIME_WAIT
  // so this method used to change on a setting of a socket and its require for that the
  // socket and the level of option you want to change on that socket and the option name 
  // or value after that the value and size you want the setsockopt to take from 
  // that value
  setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  memset(&_addr, 0, sizeof(_addr));
  _addr.sin_family = AF_INET;// ipv4 family
  _addr.sin_port = htons(_port);
  // The htons() function converts the unsigned short integer hostshort from
  // host byte order to network byte order.
  _addr.sin_addr.s_addr = inet_addr(_ip.c_str());
  // convert from string to 32bit format
  if (bind(_socket_fd, (struct sockaddr *)(&_addr), sizeof(_addr)))
  {
    // bind is like setsockopt its just tell the os that the socket belongs
    // to this address and this port
    throw ServerException("bind() failed with port" + to_string98(_port));
  }
  // listen()  marks  the  socket referred to by sockfd as a passive socket,
  // that is, as a socket that will be used to  accept  incoming  connection
  // requests using accept(2).
  if (listen(_socket_fd, MAX_CONX_QUEUE))
    throw ServerException("listen() failed.");

  std::cout << INFO_MSG << "listen on http://" << inet_ntoa(_addr.sin_addr) << ":" << to_string98(ntohs(_addr.sin_port)) << std::endl;
}

void Server::_handelClient(socklen_t size_socket)
{
  int n = epoll_wait(_epoll_fd, _events, MAX_EVENTS, EPOLL_WAIT_TIMEOUT);
  if (n == -1)
  {
    DEBUG_ERROR("handelClient: epoll wait fail");
    return ;
  }
  for (int i = 0; i < n; i++)
  {
    // new client
    if (_events[i].data.fd == _socket_fd)
    {
      int client_fd = accept(_socket_fd, (struct sockaddr *)(&_addr), (socklen_t *)&size_socket);
      DEBUG_INFO("------------New Request-----------");
      if (client_fd < 0)
        throw ServerException("accept() failed.");
      // adding to clinet list
      _clients.addClient(client_fd);
      this->_addClient(client_fd);
    }
    // EPOLLIN fires on client_fd:
    else if (_events[i].events & EPOLLIN)
    {
      Client *cl = _clients.getClient(_events[i].data.fd);
      if (!cl){/*TODO:erroo*/ return;}
      
      // read client headers
      //n = read(fd, buf, sizeof(buf))

      // user close connection n == 0  → EOF       → disconnect()
      // error with socket or internet conneciton n == -1 → error     → disconnect()
      // n > 0   → got data  → check client state:
/*
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




      // TODO: handle client I/O
      if (cl->getState() == READING_HEADERS)
        this->readheaders(cl);
      else if (cl->getState() == READING_BODY) {
        if (req.getHeaderValue("Content-Type") == "multipart/form-data")
          {}// TODO: store on tmp file
        else {}
          // TODO: store on string
     /* READING_BODY:
      write chunk to client.tmp_file (random.tmp)
      client.bytes_read += n
      if bytes_read < Content-Length → stay in READING_BODY
      if bytes_read == Content-Length →
        close tmp_file
        transition to PROCESSING
        */
      }
      else if (cl->getState() == PROCESSING) {
          DEBUG_INFO("Response");
          Response res(req);
          std::string result = res.build();
         // write back to user fd
          // close
/*PROCESSING:
      build response headers
      DELETE → execute delete, transition to RESPONDING
      GET    → find file, transition to RESPONDING
      POST   → move tmp_file to final location, transition to RESPONDING
      re-register fd for EPOLLOUT*/
      }
      // TODO: after done from processing register client as EPOLLOUT using epoll_ctl(MOD)
      // update clinet last activity to now
      cl->updateLastActivity();
    }
    else if (_events[i].events & EPOLLOUT)
    {
      Client *cl = _clients.getClient(_events[i].data.fd);
      if (!cl){/*TODO:erroo*/ return;}

      /*EPOLLOUT fires on client_fd:
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
      // even if this should not happen i will just check for edge cases 
      if (cl->getState() != SENDING)
        return ;

      // TODO: send that to client and close connection after done
      // send();
      cl->setState(DONE);
      _clients.disconnect(cl->getFd());

    }
    else if (_events[i].events & EPOLLHUP || _events[i].events & EPOLLERR)
    {
      // client disconnected or error
      _clients.disconnect(_events[i].data.fd);
    }
  }
  // INFO: checking timeout everytime can reduce performance
  // check timeout n=0
  _clients.checkTimeout();
}

