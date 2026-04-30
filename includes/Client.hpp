#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "EpollHold.hpp"
#include "Request.hpp"
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
        uint32_t    _epoll_events;
        int _epfd;
        std::string  _readBuffer;
        std::string  _writeBuffer;
        size_t       _writeOffset;
        time_t       _lastActivity;
        ClientState  _state;
        t_epollhold  _sock;
        Request     _req;
        bool _is_cgi;
        t_epollhold _clsock_hold;
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
        bool                isCGI()          const;
        t_epollhold&         getClSockHolder()       const;
        
        // setters
        void  setState(ClientState state);
        void  setWriteBuffer(const std::string& data);
        void  setReadBuffer(const std::string& data);
        void  updateLastActivity();
        void  setIsCGI(bool value);
        void invalidateFd();

        // methods
        void  appendToReadBuffer(const char* data, size_t len);
        void  advanceWriteOffset(size_t bytes);
        void  clearReadBuffer();
        void  clearWriteBuffer();
        bool  isTimedOut(int timeoutSeconds) const;
        virtual void  disconnect(int epfd);
    protected:
        Client(const Client& cl);
        Client& operator=(const Client&);

};

#endif
