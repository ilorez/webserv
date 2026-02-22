/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:40:49 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/22 16:09:21 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "iostream"
#include <vector>

std::string ft_readFile(std::string src);
size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);

#endif
