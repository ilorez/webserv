/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:48:00 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/22 15:07:53 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include "../includes/Debug.hpp"

void Request::request_pars(std::string request)
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
  std::cout << INFO_MSG << "line : " << raws[0] << std::endl;
  // get method and route and http version
  std::vector<std::string> fields;
  split(raws[0], fields, ' ');
  // Debug
  std::cout << INFO_MSG << "method: " << fields[0] << "" << std::endl;
  std::cout << INFO_MSG << "route: " << fields[1] << "" << std::endl;
  std::cout << INFO_MSG << "http ver: " << fields[2] << "" << std::endl;
  this->method = fields[0];
  this->route = fields[1];
}
