/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/29 12:57:16 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/22 15:14:34 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include "./colors.hpp"

/*
** Enable debug by compiling with:
**   -DDEBUG
**
** Example:
**   c++ -Wall -Wextra -Werror -DDEBUG *.cpp
*/

#ifdef DEBUG

    #define DEBUG_INFO(msg) \
        std::cout << INFO_MSG << msg << std::endl

    #define DEBUG_WARN(msg) \
        std::cerr << WARNING_MSG << msg << std::endl

    #define DEBUG_ERROR(msg) \
        std::cerr << ERROR_MSG << msg << std::endl

#else

    #define DEBUG_INFO(msg)   ((void)0)
    #define DEBUG_WARN(msg)   ((void)0)
    #define DEBUG_ERROR(msg)  ((void)0)

#endif

#endif
