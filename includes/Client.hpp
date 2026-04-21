#ifndef CLIENT_HPP
#define CLIENT_HPP

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
        std::string  _readBuffer;
        std::string  _writeBuffer;
        size_t       _writeOffset;
        time_t       _lastActivity;
        ClientState  _state;
        Request     _req;
        bool _is_cgi;
    public:
        Client(int fd);
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
