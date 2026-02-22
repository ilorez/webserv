/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:48:30 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/22 14:34:27 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <sys/socket.h>   // socket(), bind(), listen(), accept() #include <netinet/in.h>   // struct sockaddr_in, htons(), INADDR_ANY
#include <unistd.h>       // read(), write(), close()
#include <arpa/inet.h>    // inet_addr() — optional for now
#include <vector>
//
#define MAX_CONX_QUEUE 10
#define BUF_SIZE 1024

size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
}

void request_pars(std::string request)
{
  std::string delimiter = "\r\n";
  std::vector<std::string> raws;

  size_t pos = 0; 
  while ((pos = request.find(delimiter)) != std::string::npos)
  {
      raws.push_back(request.substr(0, pos));
      request.erase(0, pos+delimiter.length());
  }
  // Debug
  std::cout << "line : " << raws[0] << std::endl;
  // get method and route and http version
  std::vector<std::string> fields;
  split(raws[0], fields, ' ');
  // Debug
  std::cout << "method: " << fields[0] << "" << std::endl;
  std::cout << "route: " << fields[1] << "" << std::endl;
  std::cout << "http ver: " << fields[2] << "" << std::endl;
}

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (fd < 0)
  {
    std::cout << "Error with socket" << std::endl;
    return (1);
  }

  int opt = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  if (bind(fd, (struct sockaddr*)(&addr), sizeof(addr)))
  {
    std::cout << "Error with bind" << std::endl;
    return 2;
  }

  if (listen(fd, MAX_CONX_QUEUE))
  {
    std::cout << "Error with listen" << std::endl;
    return (3);
  }
  std::cout << "Listning on: http:://" << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;

  socklen_t size_socket = sizeof(addr);
  while (1)
  {
    int accpetFd = accept(fd, (struct sockaddr*)(&addr), (socklen_t *)&size_socket);
    if (accpetFd < 0)
    {
      std::cout << "Error with accept" << std::endl;
      return (4);
    }
    char buf[BUF_SIZE]; 
    memset(&buf, 0, BUF_SIZE);
    // read from user fd
    size_t readed = read(accpetFd, buf, BUF_SIZE);
    std::cout << "Request content" << std::endl;
    std::cout << "size: " << readed << std::endl;
    std::cout << buf << std::endl;
    std::cout << "------ Request end -------" << std::endl;
    request_pars(buf);
    std::ifstream readfile("./www/index.html");
    if (!readfile.is_open())
    {
      std::cout << "Error with ifstream read file" << std::endl;
      return (5);
    }
    std::string line;
    std::string body;
    while (std::getline(readfile, line))
      body += line;
    readfile.close();
    std::string response = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/html\r\n"
                       "Content-Length: " + std::to_string(body.length()) + "\r\n"
                       "\r\n"// this how reader know the end of header
                       + body;
    // write back to user fd
    write(accpetFd, response.c_str(), response.length());
    // close 
    close(accpetFd);
  }
  close(fd);
}
