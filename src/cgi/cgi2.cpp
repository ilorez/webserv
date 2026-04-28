
// ----------------------------------------------------------------------------------
// ============================================================
// CGI ARCHITECTURE - UPDATED DESIGN
// ============================================================
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
