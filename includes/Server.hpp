#ifndef SERVER_HPP
#define SERVER_HPP


#include "ManageClients.hpp"
#include "Request.hpp"
#include "settings.hpp"
#include <arpa/inet.h>    // inet_addr() — optional for now
#include <sys/epoll.h>
#include <sys/socket.h>   // socket(), bind(), listen(), accept() #include <netinet/in.h>   // struct sockaddr_in, htons(), INADDR_ANY
#include <string>

class Server {
  private:
    int _port;
    int _socket_fd;
    int _epoll_fd;
    std::string _ip;
    struct sockaddr_in _addr;
    ManageClients _clients;
    struct epoll_event _events[MAX_EVENTS];
    struct epoll_event _epoll_event;
    Request req;
    // ...
  public:
    // orthodox
    Server();
    ~Server();
    Server(const Server& copy);
    Server& operator=(const Server& copy);
    
    // getters and setters
    std::string getIp() const;
    std::string getPort() const;
  
    // parse config file
    void parseConfig(); // parse info from config file

    // read headers
    void readheaders(Client *cl);

    // new connection
    void newconnection(socklen_t size_socket);
    // i/o
    void readrequest(Client *cl);
    void sendresponse(Client *cl);
    bool createTmpFile(Client *cl);
    void  readFromSocket(Client *cl);
    void  readFromSocket(Client *cl, int);
    void sendFromFile(Client *cl, int file_fd);
    
    //create socket 
    void run(); // create socket and start listning
    private:
      void _initSocket();
      void _handelClient(socklen_t);
      void _addClient(int client_fd);
      // TODO:handelReadyClient
      // TODO:_client_disconnected
      void _handelClientDisconnect();
      void _switchEpollRegisration(int client_fd, uint32_t ev);
};

#endif
