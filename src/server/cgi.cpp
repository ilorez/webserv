
#include "../../includes/container.hpp"

CGIClient::CGIClient(int fd): Client(fd), _child_pid(-1), _cgi_headers_parsed(false), _download_switch(true), _upload_switch(false){
}
CGIClient::CGIClient(Client &cl): Client(cl), _child_pid(-1), _cgi_headers_parsed(false){
  cl.invalidateFd(); // stop ~Client() closing the _fd
}

CGIClient::~CGIClient()
{
  DEBUG_INFO("CGIClient disructor called");
  // TODO
  // kill
  // wait pids
  // close pipes
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

void CGIClient::disconnect(int epfd)
{
  (void) _pipe_in, (void)_pipe_out, (void)_child_pid, (void)_cgi_headers_parsed;
  (void) _download_switch, (void) _upload_switch;
  // TODO
  // unrigister pipes from epoll
  // close pipes
  // kill process if not already killed
  Client::disconnect(epfd);
}




// create pipe
// register pipe inpute fd to epoll in case of there is something in buff && post 
// register pipe output with setting EPOLLIN 
// close pipe input in case of there is no thing to send to 
// fork
// fd > 0 => run script
// fd ==0 => 
//
// EPOLLOUT in pipe fd => u need to write
//   - take the readed chunk from post body and send it 
//   - if buffer is empty remove pipe epollout from epoll
// EPOLLIN in pipe fd => u need to read
//   - put script output in std::string writeBuffer (don't store more then 1mb)
// 
// EPOOLIN request socket
//   - in other side when epollin comes for cgi, and read a body into buffer
//   - you should always make sure that the pipe is registred EPOLLOUT
// EPOOLOUT reqeuset socket
//   - yes here too you need to make sure that you don't registed the
//   - socket in epool until there is something in writeBuffer
//   
//
//
// for now my solution for register and unregister the fds of cgi setuation is using
// just simple flags in my clients class, because the cgi it self will be a client
// but wait is good to add all that attributes to the Client ??
// should i use inhertace
// like cgi is inhert from Client and add somethings to it
// but how i can now that client is cgi, emmm there is a way i'm sure about that i will ask teamate later
// what i need in cgi child class
// first, register and unregister methods
// link between all the fds, so i can register pipe fd in epoll when somehting data comes in socket fd
// also some flags for check, like isReady
// cgi disconnect is different, because if something happen i need  close fds kill child process and disconnect the client cgi also and also before that unregister the pipe fds from epoll
// NOTE: take care of the loop of epoll that looping on ready events, for CGI the first events can be realted to last event so if you close the pipe fds you may find a probleme, ask your teamate about also if you are not sure
// you can know if request is cgi by check clinet.req.isCGI so after that you can work with pointer as CGI class and call methods you need
//
//
//
// ofcourse in middels of all this there is somethings you need to add later like envirnoment varibales ...
// also all reads/writes in chunks style
// 
//
//
//






// ----------------------------------------------------------------------------------
// ============================================================
// CGI ARCHITECTURE - UPDATED DESIGN
// ============================================================
//
// CLASS DESIGN
// ------------
// CGIClient : public Client
//   - int pipe_in[2]      (parent writes → CGI stdin)
//   - int pipe_out[2]     (parent reads  ← CGI stdout)
//   - pid_t child_pid
//   - bool cgi_headers_parsed
//   - virtual disconnect() → kill child, close + unreg pipe fds, then base cleanup
//
// HOW TO DETECT CGI IN EVENT LOOP
// --------------------------------
// CGIClient* cgi = dynamic_cast<CGIClient*>(clients[fd]);
// if (cgi != NULL) { /* CGI-specific handling */ }
//
// ============================================================
// SETUP (when request is identified as CGI)
// ============================================================
//
// - create pipe_in[2]  and pipe_out[2]
// - fork()
//
//   CHILD (pid == 0):
//     - close pipe_in[1]  (don't need write end)
//     - close pipe_out[0] (don't need read end)
//     - dup2(pipe_in[0],  STDIN_FILENO)
//     - dup2(pipe_out[1], STDOUT_FILENO)
//     - close pipe_in[0] and pipe_out[1] (already duped)
//     - set environment variables (REQUEST_METHOD, CONTENT_LENGTH, etc.)
//     - execve(script_path, args, env)
//     - exit(1) if execve fails
//
//   PARENT (pid > 0):
//     - close pipe_in[0]  (don't need read end)
//     - close pipe_out[1] (don't need write end)
//     - if POST (body to send): register pipe_in[1]  with EPOLLOUT
//     - always:               register pipe_out[0] with EPOLLIN
//     - store CGIClient in map, keyed by socket fd
//     - also store pipe fds in map so epoll events can find the CGIClient
//
// ============================================================
// EPOLL EVENT LOOP
// ============================================================
//
// EPOLLOUT on pipe_in[1]  (parent → CGI stdin)
//   - look up CGIClient via pipe fd map
//   - write next chunk of POST body to pipe
//   - if POST body fully sent:
//       - unregister pipe_in[1] from epoll
//       - close pipe_in[1]  ← signals EOF to CGI script
//
// EPOLLIN on pipe_out[0]  (CGI stdout → parent)
//   - look up CGIClient via pipe fd map
//   - read chunk into CGIClient.writeBuffer (cap at 1MB)
//   - if read() == 0 (CGI done writing):
//       - unregister pipe_out[0] from epoll
//       - close pipe_out[0]
//       - waitpid(child_pid, ...) to reap zombie
//       - parse CGI headers from writeBuffer if not yet parsed
//       - register socket fd with EPOLLOUT to send response to client
//
// EPOLLIN on socket fd  (client → server, POST body chunks)
//   - read chunk into CGIClient.postBuffer
//   - make sure pipe_in[1] is registered EPOLLOUT if not already
//
// EPOLLOUT on socket fd  (server → client, send CGI response)
//   - only register this AFTER writeBuffer has data
//   - send chunk from writeBuffer to client
//   - if fully sent: disconnect()
//
// ============================================================
// CGIClient::disconnect()  (virtual override)
// ============================================================
//
// - if child_pid still alive: kill(child_pid, SIGKILL) + waitpid
// - unregister + close pipe_in[1]  if still open
// - unregister + close pipe_out[0] if still open
// - remove pipe fds from pipe fd map
// - call base Client::disconnect() → unreg + close socket fd
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
// - pipe fds must be set non-blocking (fcntl O_NONBLOCK) like socket fds
