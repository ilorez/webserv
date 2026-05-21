#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "EpollHold.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <string>
#include <ctime>

enum ClientState
{
    READING_HEADERS = 0,
    READING_BODY,
    PROCESSING,
    SENDING,
    DONE
};


class Client
{
    protected:
        int          _fd;
        int          _epfd;
        int          _epoll_events;
        std::string  _readBuffer;
        std::string  _writeBuffer;
        size_t       _writeOffset;
        time_t       _lastActivity;
        ClientState  _state;
        Request      _req;
        t_epollhold  _clsock_hold;
        int          _status_error; // if 0 there is no error // send error to res.build(_status_error) to create response base on error  
        Response     _res;
    public:
        Client(int fd, int epfd);
        virtual ~Client();

        // getters
        int                 getFd()          const;
        const std::string&  getReadBuffer()  const;
        const std::string&  getWriteBuffer() const;
        size_t              getWriteOffset() const;
        time_t              getLastActivity() const;
        ClientState         getState()       const;
        Request&            getReq();
        t_epollhold&        getClSockHolder();
        
        // setters
        void  setState(ClientState state);
        void  setWriteBuffer(const std::string& data);
        void  setReadBuffer(const std::string& data);
        void  updateLastActivity();
        void  forceTimeout();
        void  invalidateFd();
        virtual void handel(int, uint32_t evs);

        // on error
        void callError(int err_code);


        // io
        std::string readHeaders();
        virtual void preSetup();

        void  readbody();
        void  readFromSocket();
        void  readFromSocket(int);

        void  processing();
        void  sendResponse();

        // methods
        bool  createTmpFile();
        void  appendToReadBuffer(const char* data, size_t len);
        void  advanceWriteOffset(size_t bytes);
        void  clearReadBuffer();
        void  clearWriteBuffer();
        bool  isTimedOut(int timeoutSeconds) const;
        void  switchToEpollOut();
    protected:
        Client(const Client& cl);
        Client& operator=(const Client&);
};

#endif
