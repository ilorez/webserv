#ifndef SERVER_HPP
#define SERVER_HPP


#include <arpa/inet.h>    // inet_addr() — optional for now
#include <sys/socket.h>   // socket(), bind(), listen(), accept() #include <netinet/in.h>   // struct sockaddr_in, htons(), INADDR_ANY
#include <string>

class Server {
  private:
    int _port;
    std::string _ip;
    int _socket_fd;
    struct sockaddr_in _addr;
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
      void _handelClient(socklen_t);
};

#endif
