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

Response::Response(Request req)
  : _req(req)
{}

Response::Response(const Response &other) : _req(other._req)
{}

Response &Response::operator=(const Response &other)
{
  if (this != &other)
  {
    this->_req = other._req;
  }
  return (*this);
}

Response::~Response() {};

std::string timespecToString(time_t tv_sec, long tv_nsec)
{
    std::tm *tm_info = std::localtime(&tv_sec);

    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);

    std::ostringstream oss;
    oss << buffer
        << "." << std::setw(9) << std::setfill('0') << tv_nsec;

    return oss.str();
}

std::string returnFileExtension(std::string path)
{
  std::string result;
  size_t pos = path.rfind('.');
  if (pos != std::string::npos)
  {
      result = path.substr(pos);
  }
  return (result);
}

std::string Response::returnMediaType(std::string path)
{
  std::map<std::string, std::string>::iterator it = _mapMediaTypes.find(returnFileExtension(path));
  return (it->second);
}

void Response::initHeaders(std::map<std::string, std::string> &h)
{
  struct stat _stat;

  int rv = stat(_req.getPath().c_str(), &_stat);
  if (rv == -1)
    throw ResponseException("stat failure\n");

  h.insert(std::make_pair("Server : ", "MyServer"));
  h.insert(std::make_pair("Date : ", timespecToString(_stat.st_ctim.tv_sec, _stat.st_ctim.tv_nsec)));
  h.insert(std::make_pair("Content-Length : ", to_string98(_stat.st_size)));
  h.insert(std::make_pair("Content-Type : ", returnMediaType(_req.getPath())));
}

std::string  Response::mergeResponseToString()
{
  std::string ret;
  std::map<int, std::string>::iterator sc_it = _mapStatusCodes.find(_status);

  // make start line
  ret += _req.getVersion() + " " + to_string98(sc_it->first) + sc_it->second + "\r\n";
  
  // make headers
  for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
       it != _headers.end(); ++it) {
      ret += it->first + it->second + "\n";
  }

  // empty line 
  ret += "\r\n";

  // set the body
  ret += _body;

  return (ret);
}

std::string Response::build()
{
  std::string response;

  initStatusCodes(_mapStatusCodes);
  initMediaTypes(_mapMediaTypes);


  _body = ft_readFile(_req.getPath());
  if (_body.empty())
  {
      _status = 404;
      _req.setPath("./www/404.html");
      _body = ft_readFile(_req.getPath());
      if (_body.empty())
      {
        throw ResponseException("Page 404.html does not exist");
      }
  }
  else
    _status = 200;

  initHeaders(_headers);
  response = mergeResponseToString();
  return response;
}

void Response::initStatusCodes(std::map<int, std::string> &m)
{
  m.insert(std::make_pair(200, " OK"));
  m.insert(std::make_pair(201, " Created"));
  
  m.insert(std::make_pair(301, " Moved Permanently"));
  m.insert(std::make_pair(302, " Found"));

  m.insert(std::make_pair(400, " Bad Request"));
  m.insert(std::make_pair(401, " Unauthorized"));
  m.insert(std::make_pair(403, " Forbidden"));
  m.insert(std::make_pair(404, " Not Found"));

  m.insert(std::make_pair(500, " Internal Server Error"));
  m.insert(std::make_pair(502, " Bad Gateway"));
  m.insert(std::make_pair(503, " Service Unavailable"));
}

void Response::initMediaTypes(std::map<std::string, std::string> &m)
{
  m.insert(std::make_pair(".txt",  "text/plain"));
  m.insert(std::make_pair(".html", "text/html"));
  m.insert(std::make_pair(".png",  "image/png"));
  m.insert(std::make_pair(".jpg",  "image/jpeg"));
  m.insert(std::make_pair(".json", "application/json"));
}
