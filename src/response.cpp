/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 23:48:09 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/23 00:08:46 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/Response.hpp"
#include "../includes/ToString.hpp"
#include <string>

Response::Response(int status, const std::string& body, const std::string& contentType)
{
  this->_status = status;
  this->_body = body;
  this->_contentType = contentType; // text/html
}


std::string Response::build() const
{
  std::string status;
  if (_status == 200)
    status = "200 OK";
  else
    status = "404 Not Found";
  std::string response = "HTTP/1.1 " + status + "\r\n"
                     "Content-Type: " + _contentType + "\r\n"
                     "Content-Length: " + to_string98(_body.length()) + "\r\n"
                     "\r\n"// this how reader know the end of header
                     + this->_body;
  return response;
}

