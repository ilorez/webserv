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

#include <string>
class Response
{
private:
    int         _status;
    std::string _body;
    std::string _contentType;

public:
    Response(int status, const std::string& body, const std::string& contentType);

    std::string build() const;
};

#endif
