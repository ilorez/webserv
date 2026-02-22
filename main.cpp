/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:48:30 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/22 13:27:30 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <cstring>
#include <iostream>
#include <sys/socket.h>   // socket(), bind(), listen(), accept() #include <netinet/in.h>   // struct sockaddr_in, htons(), INADDR_ANY
#include <unistd.h>       // read(), write(), close()
#include <arpa/inet.h>    // inet_addr() — optional for now
//
#define MAX_CONX_QUEUE 10
#define BUF_SIZE 1024

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
    char buf[1024]; 
    memset(&buf, 0, BUF_SIZE);
    // read from user fd
    size_t readed = read(accpetFd, buf, BUF_SIZE);
    std::cout << "Request content" << std::endl;
    std::cout << "size: " << readed << std::endl;
    std::cout << buf << std::endl;
    std::cout << "------ Request end -------" << std::endl;
    std::string body = "<h1>Hello World</h1>";
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
