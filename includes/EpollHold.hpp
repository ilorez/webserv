#ifndef EPOLLHOLD
#define EPOLLHOLD


class Client;
class CGIClient;
class ServerConfig;

typedef struct s_epollhold
{
  union {
    CGIClient *cgi;
    Client *cl;
    ServerConfig *sc;
  };
  int fd;
  bool is_cgi;
} t_epollhold;


#endif
