#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>

enum ClientState
{
    READING_HEADERS,
    READING_BODY,
    PROCESSING,
    SENDING,
    DONE
};

class Client
{
    private:
        int          _fd;
        std::string  _readBuffer;
        std::string  _writeBuffer;
        size_t       _writeOffset;
        time_t       _lastActivity;
        ClientState  _state;

    public:
        Client(int fd);
        ~Client();

        // getters
        int                 getFd()          const;
        const std::string&  getReadBuffer()  const;
        const std::string&  getWriteBuffer() const;
        size_t              getWriteOffset() const;
        time_t              getLastActivity() const;
        ClientState         getState()       const;

        // setters
        void  setState(ClientState state);
        void  setWriteBuffer(const std::string& data);
        void  setReadBuffer(const std::string& data);
        void  updateLastActivity();

        // methods
        void  appendToReadBuffer(const char* data, size_t len);
        void  advanceWriteOffset(size_t bytes);
        void  clearReadBuffer();
        void  clearWriteBuffer();
        bool  isTimedOut(int timeoutSeconds) const;
    private:
        Client(const Client&);
        Client& operator=(const Client&);

};

#endif
