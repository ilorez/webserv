#ifndef SERVER_HPP
#define SERVER_HPP


#include "Client.hpp"
#include "EpollHold.hpp"
#include "ManageClients.hpp"
#include "Request.hpp"
#include "Settings.hpp"
#include <arpa/inet.h>    // inet_addr() — optional for now
#include <sys/epoll.h>
#include <sys/socket.h>   // socket(), bind(), listen(), accept() #include <netinet/in.h>   // struct sockaddr_in, htons(), INADDR_ANY
#include <string>

class Server {
  private:
    int _epoll_fd;
    bool _is_running;
    struct sockaddr_in _addr;
    ManageClients _clients;
    std::vector<ServerConfig> &_servers;
    struct epoll_event _events[MAX_EVENTS];
    struct epoll_event _epoll_event;

  public:
    // orthodox
    Server(std::vector<ServerConfig>& servers);
    ~Server();
    
    // parse config file
    void parseConfig(); // parse info from config file

    // read headers
    bool readHeaders(t_epollhold *eh);

    // new connection
    void newConnection(ServerConfig &sc);

    // i/o
    void sendResponse(Client *cl);
    void sendFromFile(Client *cl, int file_fd);
    
    //create socket 
    void run(); // create socket and start listning

    void serverFree();
  private:
    Server(const Server& copy);
    Server& operator=(const Server& copy);
    void _initSocket();
    void _initSocket(ServerConfig &sc);
    void _handelClient();
    void _addClient(int client_fd, ServerConfig &sc);
    void _addSocketToEpoll(t_epollhold &_srv_hold, int sfd); // for only socket
    void _switchEpollRegisration(t_epollhold *cl, uint32_t ev);
};

#endif
