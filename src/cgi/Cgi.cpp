#include "../../includes/container.hpp"
#include <unistd.h>

// Constructor
CGIClient::CGIClient(int fd, int epfd)
    : Client(fd, epfd),
      _pid(-1),
      _socket_done(false),
      _cgi_pipe_done(false),
      _read_counter(0),
      _pipe_in_hold(),
      _pipe_out_hold(),
      _got_headers_end(false)
{
  _pipe_in[0] = -1;
  _pipe_in[1] = -1;
  _pipe_out[0] = -1;
  _pipe_out[1] = -1;
  _clsock_hold.is_cgi = true;
  _clsock_hold.cgi = this;
  _pipe_in_hold.is_cgi = true;
  _pipe_in_hold.cgi = this;
  _pipe_out_hold.is_cgi = true;
  _pipe_out_hold.cgi = this;
}

// Steal constructor
CGIClient::CGIClient(Client &cl)
    : Client(cl),
      _pid(-1),
      _socket_done(false),
      _cgi_pipe_done(false),
      _read_counter(0),
      _pipe_in_hold(),
      _pipe_out_hold(),
      _got_headers_end(false)
{
  _pipe_in[0] = -1;
  _pipe_in[1] = -1;
  _pipe_out[0] = -1;
  _pipe_out[1] = -1;
  _clsock_hold.is_cgi = true;
  _clsock_hold.cgi = this;
  _pipe_in_hold.is_cgi = true;
  _pipe_in_hold.cgi = this;
  _pipe_in_hold.fd = -1;
  _pipe_out_hold.is_cgi = true;
  _pipe_out_hold.cgi = this;
  _pipe_out_hold.fd = -1;
  cl.invalidateFd(); // stop ~Client() closing the _fd
}

CGIClient::~CGIClient()
{
  DEBUG_INFO("CGIClient disructor called");
  int status;

  // unregistred pipes
  // idon't need to remove theme because closing the fd will do the work
  //epoll_ctl(_epfd, EPOLL_CTL_DEL, _pipe_in[1], NULL);
  //epoll_ctl(_epfd, EPOLL_CTL_DEL, _pipe_out[0], NULL);

  // kill
  if (_pid != -1)
  {
    kill(_pid, SIGKILL);
    // waitpid
    waitpid(_pid, &status, 0);
    // DEBUG_INFO2("status exit: " + to_string98(status));
    check_process_status(status);
  }

  // close pipes
  ft_closefd(_pipe_in[1]);
  ft_closefd(_pipe_out[0]);
}

// operator=
CGIClient &CGIClient::operator=(const CGIClient &other)
{
  if (this == &other)
    return *this;
  Client::operator=(other);
  _pid = other._pid;
  _socket_done = other._socket_done;
  _cgi_pipe_done = other._cgi_pipe_done;
  _read_counter = other._read_counter;
  _pipe_in[0] = other._pipe_in[0];
  _pipe_in[1] = other._pipe_in[1];
  _pipe_out[0] = other._pipe_out[0];
  _pipe_out[1] = other._pipe_out[1];
  _clsock_hold.is_cgi = true;
  _clsock_hold.cgi = this;
  _pipe_in_hold.is_cgi = true;
  _pipe_in_hold.cgi = this;
  _pipe_in_hold.fd = other._pipe_in_hold.fd;
  _pipe_out_hold.is_cgi = true;
  _pipe_out_hold.cgi = this;
  _pipe_out_hold.fd = other._pipe_out_hold.fd;
  _got_headers_end = other._got_headers_end;
  return *this;
}

// Copy constructor logic in operator= constructor
CGIClient::CGIClient(const CGIClient &other)
    : Client(other),
      _pid(-1),
      _socket_done(false),
      _cgi_pipe_done(false),
      _read_counter(0),
      _pipe_in_hold(),
      _pipe_out_hold(),
      _got_headers_end(false)
{
  _pipe_in[0] = -1;
  _pipe_in[1] = -1;
  _pipe_out[0] = -1;
  _pipe_out[1] = -1;
  *this = other;
}

void CGIClient::setupPipes()
{
  if (pipe(_pipe_in) != 0 || pipe(_pipe_out) != 0)
  {
    DEBUG_ERROR("pipe failed");
    this->setState(DONE);
    throw CGIException("pipe error", 1);
  }
  _pipe_in_hold.fd = _pipe_in[1];
  _pipe_out_hold.fd = _pipe_out[0];
  // DEBUG_INFO("PIPEs has been setuped");
}

void CGIClient::registerPipeOut()
{
  DEBUG_INFO("registerPipeOut called");
  // DEBUG_INFO("my fd is: " + to_string98(_pipe_out[0]));
  //  applying non-blocking
  fcntl(_pipe_out[0], F_SETFL, O_NONBLOCK);

  // create epoll holder
  // setup event
  struct epoll_event ev = create_ev(&_pipe_out_hold, EPOLLIN);

  // adding to epoll queu
  epoll_ctl(_epfd, EPOLL_CTL_ADD, _pipe_out[0], &ev);
}

// when sending input to cgi script
void CGIClient::registerPipeIn()
{
  // applying non-blocking
  fcntl(_pipe_in[1], F_SETFL, O_NONBLOCK);

  struct epoll_event ev = create_ev(&_pipe_out_hold, EPOLLOUT);

  // adding to epoll queu
  epoll_ctl(_epfd, EPOLL_CTL_ADD, _pipe_in[1], &ev);
}

void ft_change_fd(int fd, int to)
{
  if (fd == to)
    return;
  if (dup2(fd, to) == -1)
    throw CGIException("dup2 failed", 1);
  close(fd);
}

void CGIClient::ftExec()
{
  _pid = fork();
  if (_pid == -1)
    throw CGIException("fork failed", 1);
  if (_pid == 0)
  {
    // child
    close(_pipe_in[1]);
    close(_pipe_out[0]);
    ft_change_fd(_pipe_in[0], STDIN_FILENO);
    ft_change_fd(_pipe_out[1], STDOUT_FILENO);

    std::string cgiPath = _req.getCgiPath();
    std::string scriptPath = _req.getFilePath();
    char *argv[] = {
      const_cast<char*>(cgiPath.c_str()),
      const_cast<char*>(scriptPath.c_str()),
      NULL
    };
    char **env = buildEnv();
    execve(argv[0], argv, env);
    write(STDOUT_FILENO, "Status: 502 Bad Gateway\n\n", 25);
    freeEnv(env);
    _pid = -1;
    _pipe_in[0] = -1;
    _pipe_out[0] = -1;
    throw CGIException("NOTHING", 127);
  }
  // parent
  ft_closefd(_pipe_in[0]);
  ft_closefd(_pipe_out[1]);
  // registed pipe out
  registerPipeOut();
  if (_req.getMethod() != "POST")
    ft_closefd(_pipe_in[1]);
}

void CGIClient::handel(int fd, uint32_t evs)
{
  try
  {
    if (evs & EPOLLIN)
    {
      // socket
      if (fd == this->_fd) // 1
        writeToReadBuffer();
      // pipe output
      else if (fd == this->_pipe_out[0]) // 3
        // this at first time is registred in epoll
        // after fired read output an put it into write buffer
        // and register socket output
        writeToWriteBuffer();
    }
    if (evs & EPOLLOUT)
    {
      // socket
      if (fd == this->_fd) // 4
        // read from write buffer and put in socket
        // if write buffer is empty
        // remove EPOOLLOUT from epoll events
        // and register againt the pipe out in case of cgi not end
        // if end // change state to DONE
        writeToSocket();
      // pipe input
      else if (fd == this->_pipe_in[1]) // 2
        // read from buffer and put into pipe
        // if buffer is empty => remove the pipe in 1 from epoll
        // and add socket again to epoll in case of content len is not end
        writeToPipe();
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << ERROR_MSG << e.what() << std::endl;
    this->forceTimeout();
    this->setState(DONE);
  }
}
