/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:48:00 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/23 00:08:35 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include "../includes/debug.hpp"

Request::Request(const std::string &raw)
{
  this->_request_pars(raw);
}


// getters
std::string Request::getMethod() const
{
  return (this->_method);
}

std::string Request::getPath() const
{
  return (this->_path);
}

std::string Request::getBody() const
{
  return (this->_body);
}

void Request::setPath(const std::string &p)
{
  if (p == "/" || p == "/index.html")
    _path = "./www/index.html";
  else
    _path = "./www/404.html";
}

// member functions
void Request::_request_pars(const std::string& raw)
{
  std::string delimiter = "\r\n";
  std::string request = raw;
  std::vector<std::string> raws;

  size_t pos = 0; 
  while ((pos = request.find(delimiter)) != std::string::npos)
  {
      raws.push_back(request.substr(0, pos));
      request.erase(0, pos+delimiter.length());
  }
  // get method and route and http version
  std::vector<std::string> fields;
  split(raws[0], fields, ' ');

  // Debug
  std::cout << INFO_MSG << "method: " << fields[0] << "" << std::endl;
  std::cout << INFO_MSG << "route: " << fields[1] << "" << std::endl;
  std::cout << INFO_MSG << "http ver: " << fields[2] << "" << std::endl;

  this->_method = fields[0];
  this->setPath(fields[1]);
  this->_body = "body sould be here";
}
