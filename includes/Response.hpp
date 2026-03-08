/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 23:39:02 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/22 23:40:49 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../includes/container.hpp"

class Response
{
private:
    std::map<std::string, std::string>  _mapMediaTypes;
    std::map<int, std::string>          _mapStatusCodes;
    std::map<std::string, std::string>  _headers;
    std::string     _body;
    Request         _req;
    int             _status;

public:
    Response(Request req);
    Response(const Response &other);
    Response &operator=(const Response &other);
    ~Response();

    void initStatusCodes(std::map<int, std::string> &m);
    void initMediaTypes(std::map<std::string, std::string> &m);
    void initHeaders(std::map<std::string, std::string> &h);
    std::string returnMediaType(std::string path);
    std::string mergeResponseToString();
    std::string build();
};

#endif
