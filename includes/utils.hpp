#ifndef UTILS_HPP
#define UTILS_HPP

#include "EpollHold.hpp"
#include "iostream"
#include <vector>

std::string ft_readFile(std::string src);
size_t split(const std::string &txt, std::vector<std::string> &strs, std::string del);
std::string trim(const std::string &src);
char toLowerCase(unsigned char c);
bool endsWith(const std::string& fullString, const std::string& ending);

std::string makeTmpPath(int cl_fd);
struct epoll_event create_ev(t_epollhold *eh, uint32_t ev);

void ft_closefd(int &fd);

bool isValidPath(const std::string &path);
bool isValidStatusCode(const std::string &str);
void errorMsg(const std::string &msg, size_t line);

std::string getFileName(const std::string& path);
#endif
