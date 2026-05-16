/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: znajdaou <znajdaou@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/30 15:08:51 by znajdaou          #+#    #+#             */
/*   Updated: 2026/05/16 10:20:18 by znajdaou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/container.hpp"


CGIClient::CGIClient(int fd, int epfd): Client(fd, epfd), _pid(-1), _socket_done(false), _cgi_pipe_done(false)
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

CGIClient::CGIClient(Client &cl): Client(cl){
  _pipe_in[0] = -1;
  _pipe_in[1] = -1; _pipe_out[0] = -1;
  _pipe_out[1] = -1;
  _clsock_hold.is_cgi = true;
  _clsock_hold.cgi = this;
  _pipe_in_hold.is_cgi = true;
  _pipe_in_hold.cgi = this;
  _pipe_out_hold.is_cgi = true;
  _pipe_out_hold.cgi = this;
  cl.invalidateFd(); // stop ~Client() closing the _fd
}

CGIClient::~CGIClient()
{
  DEBUG_INFO("CGIClient disructor called");
  int status;

  // unregistred pipes
  epoll_ctl(_epfd, EPOLL_CTL_DEL, _pipe_in[1], NULL);
  epoll_ctl(_epfd, EPOLL_CTL_DEL, _pipe_out[0], NULL);

  // kill
  if (_pid != -1)
    kill(_pid, SIGKILL);
  // waitpid
  waitpid(_pid, &status, 0);

  // close pipes
  ft_closefd(_pipe_in[1]);
  ft_closefd(_pipe_out[0]);
}

// its private you can't use this 
CGIClient::CGIClient(const CGIClient &other): Client(other)
{
	(void)other;
}
CGIClient &CGIClient::operator=(const CGIClient &other)
{
	(void)other;
	return (*this);
}

void CGIClient::setupPipes()
{
  if (pipe(_pipe_in) != 0 || pipe(_pipe_out) != 0)
  {
    DEBUG_ERROR("pipe failed");
    this->setState(DONE);
    throw CGIException("pipe error");
  }
  _pipe_in_hold.fd = _pipe_in[1];
  _pipe_out_hold.fd = _pipe_out[0];
  DEBUG_INFO("PIPEs has been setuped");
}

void CGIClient::registerPipeOut()
{
  // applying non-blocking
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

void	ft_change_fd(int fd, int to)
{
	if (fd == to)
		  return;
  if (dup2(fd, to) == -1)
    throw CGIException("dup2 failed");
	close(fd);
}

void CGIClient::ft_exec()
{
  setupPipes();
  _pid = fork();
  if (_pid == -1)
    throw CGIException("fork failed");
  if (_pid == 0)
  {
    // child
    close (_pipe_in[1]);
    close (_pipe_out[0]);
    ft_change_fd(_pipe_in[0], STDIN_FILENO);
    ft_change_fd(_pipe_out[1], STDOUT_FILENO);
    char *argv[] = { (char*)"/usr/bin/python3", (char*)"./cgi-bin/hello.py", NULL };
    char *env[]  = { (char*)"REQUEST_METHOD=GET", (char*)"QUERY_STRING=name=John", NULL };
    execve("/usr/bin/python3", argv, env);
    //execve("/usr/bin/python3", argv, buildEnv());
    exit(126);
  }
  // parent
  ft_closefd(_pipe_in[0]);
  ft_closefd(_pipe_out[1]);
  // registed pipe out
  registerPipeOut();
  if (_req.getMethod() != "POST")
  {
    _epoll_events = _epoll_events & ~EPOLLIN;
    struct epoll_event ev = create_ev(&_clsock_hold, _epoll_events);
    epoll_ctl(_epfd, EPOLL_CTL_MOD, _fd, &ev);
    ft_closefd(_pipe_in[1]);
  }
}

void CGIClient::handel(int fd, uint32_t evs)
{
  // work on switch algorithm
  // if you think its not required for everything to work
  // correctly don't use it
  // but i think you need for things like adding or removing fds from the epoll
  // so i mean you need it to switch betwen reading/writing from/to socket/pipe
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


// ============================================================
// SETUP (when request is identified as CGI)
// ============================================================
//
//   CHILD (pid == 0):
//     - set environment variables (REQUEST_METHOD, CONTENT_LENGTH, etc.)
//     - execve(script_path, args, env)
//
// ============================================================
// EPOLL EVENT LOOP
// ============================================================
//
//
// EPOLLIN on pipe_out[0]  (CGI stdout → parent)
//   - if read() == 0 (CGI done writing):
//       - waitpid(child_pid, ...) to reap zombie
//       - parse CGI headers from writeBuffer if not yet parsed
//       - register socket fd with EPOLLOUT to send response to client
//
// ============================================================
// DEFERRED CLEANUP WARNING
// ============================================================
//
// Do NOT close/disconnect mid epoll_wait loop.
// epoll_wait may have already queued events for fds you just closed.
// Solution: mark CGIClient as "to be cleaned" with a flag,
// then do actual cleanup AFTER iterating all ready events.
//
// ============================================================
// ENVIRONMENT VARIABLES (to add later)
// ============================================================
//
// REQUEST_METHOD, CONTENT_LENGTH, CONTENT_TYPE,
// QUERY_STRING, PATH_INFO, SCRIPT_FILENAME, etc.
//
// ============================================================
// REMINDERS
// ============================================================
//
// - all reads/writes in chunk style
// - never buffer more than 1MB of CGI output
// - always check return of read()/write() — handle EAGAIN/EINTR
