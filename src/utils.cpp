/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 14:41:45 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/23 00:14:54 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::string ft_readFile(std::string src)
{
  std::ifstream readfile(src.c_str());
  if (!readfile.is_open())
  {
    // std::cout << "Error with ifstream read file" << std::endl;
    return "";
    /* ALAOUI: Returning NULL causes an error when checking the file */
  }
  std::string line;
  std::string all_lines;
  while (std::getline(readfile, line))
    all_lines += line;
  readfile.close();
  return all_lines;
}

size_t split(const std::string &txt, std::vector<std::string> &strs, const std::string del)
{
    size_t pos = txt.find( del );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + del.length();

        pos = txt.find( del, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos ));
    return strs.size();
}

