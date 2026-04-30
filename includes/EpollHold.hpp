#ifndef EPOLLHOLD
#define EPOLLHOLD


class Client;
class CGIClient;

typedef struct s_epollhold
{
  union {
    CGIClient *cgi;
    Client *cl;
  };
  int fd;
  bool is_cgi;
} t_epollhold;


#endif
