/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:48:30 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/22 22:31:02 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "./includes/container.hpp"


int main()
{
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
  
}
