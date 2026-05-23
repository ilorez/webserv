#include "../includes/container.hpp"
#include <sys/epoll.h>

char toLowerCase(unsigned char c)
{
  return std::tolower(c);
}

std::string getFileExtension(const std::string &path)
{
    size_t dotPos = path.rfind('.');
    if (dotPos == std::string::npos)
        return "";

    size_t slashPos = path.rfind('/');// making sure that the dot is inside a filename, not inside a directory name
    if (slashPos != std::string::npos && dotPos < slashPos)
        return "";
    return path.substr(dotPos);
}

std::string ft_readFile(std::string src)
{
  std::ifstream readfile(src.c_str());
  if (!readfile.is_open())
  {
    return "";
  }
  std::string line;
  std::string all_lines;
  while (std::getline(readfile, line))
    all_lines += line;
  readfile.close();
  return all_lines;
}

std::string trim(const std::string &src)
{
  size_t s_begin, s_end, s_range;

  s_begin = src.find_first_not_of(" ");

  if (s_begin == std::string::npos)
    return "";

  s_end = src.find_last_not_of(" ");
  s_range = s_end - s_begin + 1;

  return src.substr(s_begin, s_range);
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

bool endsWith(const std::string& fullString, const std::string& ending) {
    if (fullString.length() > ending.length()) // the requested file must be longer than the extension length 
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    else 
        return false;
}

std::string makeTmpPath(int cl_fd)
{
  std::ostringstream oss;
  oss << "/tmp/webserv_" << cl_fd << "_" << std::time(NULL) << ".tmp";
  return oss.str(); 
}
// switch epoll regitration
struct epoll_event create_ev(t_epollhold *eh, uint32_t ev)
{
  struct epoll_event r;
  r.events = ev;
  r.data.ptr = eh;
  return r;
}

void ft_closefd(int &fd)
{
  if (fd == -1)
    return;
  close(fd);
  fd = -1;
}

bool isValidPath(const std::string &path)
{
	if (path.empty() || path.find_first_of("*?[]()!<") != std::string::npos)
		return false;

	return true;
}

bool isValidStatusCode(const std::string &str)
{
	int num;

	if (str.size() != 3 || str.find_first_not_of("0123456789") != std::string::npos)
		return false;

	num = atoi(str.c_str());
	return num < 100 || num > 599 ? false : true;
}

void errorMsg(const std::string &msg, size_t line)
{
  #ifdef DEBUG
	std::ostringstream oss;
	std::string fullMsg;

	oss << "ERROR: " << msg << ". Line: " << line;
	fullMsg = oss.str();

	throw std::runtime_error(fullMsg);
  #endif
  (void)msg;
  (void)line;
}

std::string getFileName(const std::string& path)
{
    size_t pos = path.find_last_of("/");
    if (pos == std::string::npos)
        return path;
    return path.substr(pos);
}

void check_process_status(int status) {
    #ifdef DEBUG
      if (WIFEXITED(status)) {
          std::cout << "Normal exit. Code: " << WEXITSTATUS(status) << "\n";
      } 
      else if (WIFSIGNALED(status)) {
          std::cout << "Killed by signal: " << WTERMSIG(status) << "\n";
          
          #ifdef WCOREDUMP
          if (WCOREDUMP(status)) {
              std::cout << "Core dumped.\n";
          }
          #endif
      } 
      else if (WIFSTOPPED(status)) {
          std::cout << "Stopped by signal: " << WSTOPSIG(status) << "\n";
      } 
      else if (WIFCONTINUED(status)) {
          std::cout << "Continued running.\n";
      }
    #endif
    (void)status;
}

void replace_all(std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty())
        return;

    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos)
    {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}
