/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:48:30 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/22 16:07:02 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "./includes/container.hpp"


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
    Request req;
    req.request_pars(buf);
    std::string page_src;
    if (req.getRoute() != "/")
      page_src = "./www/404.html";
    else
      page_src = "./www/index.html";
    std::string body = ft_readFile(page_src);
    std::string response = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/html\r\n"
                       "Content-Length: " + to_string98(body.length()) + "\r\n"
                       "\r\n"// this how reader know the end of header
                       + body;
    // write back to user fd
    write(accpetFd, response.c_str(), response.length());
    // close 
    close(accpetFd);
  }
  close(fd);
}
