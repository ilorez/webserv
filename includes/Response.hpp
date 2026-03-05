#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
class Response
{
private:
    int         _status;
    std::string _body;
    std::string _contentType;

public:
    Response(int status, const std::string& body, const std::string& contentType);

    std::string build() const;
};

#endif
