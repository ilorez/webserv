/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:48:30 by znajdaou          #+#    #+#             */
/*   Updated: 2026/02/22 23:13:26 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "./includes/container.hpp"

int main()
{
  try {
    Server nginx;
    nginx.run();
  } catch (const ServerException &e) {
    std::cerr << ERROR_MSG << "server: " << e.what() << std::endl;
  } catch (const RequestException &e) {
    std::cerr << ERROR_MSG << "request: "<< e.what() << std::endl;
  } catch (const ResponseException &e) {
    std::cerr << ERROR_MSG << "response: "<< e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << ERROR_MSG << "error: "<< e.what() << std::endl;
  }
}
