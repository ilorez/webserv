/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ToString.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 15:37:43 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/22 16:09:37 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOSTRING_HPP
#define TOSTRING_HPP

#include <sstream>
#include <string>

template <typename T>
std::string to_string98(T value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

#endif
