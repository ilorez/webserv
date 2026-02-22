/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:37:43 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/22 15:43:22 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "iostream"

class Request {
  private:
    std::string method;
    std::string route;
  public:
    // todo: orthodox
    // getters and setters
    std::string getMethod() const;
    std::string getRoute() const;
  
    // parse request
    void request_pars(std::string request);
};

#endif
