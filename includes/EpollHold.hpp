#ifndef EPOLLHOLD
#define EPOLLHOLD

#include "CGIClient.hpp"
#include "Client.hpp"

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
