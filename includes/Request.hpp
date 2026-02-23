/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:37:43 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/23 00:03:28 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "iostream"

class Request {
  private:
    std::string _method; // get, post, delete
    std::string _path; // /, /index.html
    std::string _body; // for post
  public:
    // todo: orthodox
    Request(const std::string& raw);

    // getters and setters
    std::string getMethod() const;
    std::string getPath() const;
    std::string getBody() const;


    // set path
    void setPath(const std::string&);
  
  private:
    // parse request
    void _request_pars(const std::string& request);
};

#endif
