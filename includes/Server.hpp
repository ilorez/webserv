#ifndef SERVER_HPP
#define SERVER_HPP


#include "ManageClients.hpp"
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
    
    //create socket 
    void run(); // create socket and start listning
    private:
      void _initSocket();
      // TODO:update to handel new client
      void _handelClient(socklen_t);
      void _add_client(int client_fd);
      // TODO:handelReadyClient
      // TODO:check timeout in case of epoll wait timedout or loop have done 100+ iteration
      // TODO:_client_disconnected
};

#endif
