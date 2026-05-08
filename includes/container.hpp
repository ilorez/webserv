#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <iomanip>
#include <time.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unistd.h>
#include <map>
#include <algorithm>
#include <set>
#include <vector>
#include <cctype>   
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <csignal>
#include <sys/wait.h>
#include <sys/types.h>

#include <algorithm>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

//
#include "./settings.hpp"
#include "./colors.hpp"
#include "./debug.hpp"
#include "./settings.hpp"
#include "./WebServExceptions.hpp"

//
#include "./Templates.hpp"
#include "./utils.hpp"

//
#include "./Request.hpp"
#include "./Response.hpp"
#include "./Server.hpp"

//
#include "./Client.hpp"
#include "./CGIClient.hpp"
#include "./ManageClients.hpp"

//
#include "./Config.hpp"
#include "./Lexer.hpp"
#include "./ServerConfig.hpp"
#include "./locationConfig.hpp"
//
#include "./EpollHold.hpp"



#endif
