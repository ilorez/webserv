/* // test.cpp
#include <iostream>
#include <ostream>
struct s_test {
    union {
        int a;
        double b;
    }; // anonymous union
};

int main() {
    s_test t;
    t.a = 5;
    std::cout << "t a: " << t.a << std::endl;
    t.b = 3.3;
    std::cout << "t b: " << t.b << std::endl;
    return 0;
}
*/
#include <iostream>

class Client {
public:
    void getState() { std::cout << "Client::getState()" << std::endl; }
};

class CGIClient : public Client {
public:
    void cgiOnly() { std::cout << "CGIClient::cgiOnly()" << std::endl; }
};

int main() {
    CGIClient *cgi = new CGIClient();

    union {
        Client    *cl;
        CGIClient *cgi_ptr;
    };

    cgi_ptr = cgi;

    // accessing cl even though we stored via cgi_ptr
    cl->getState();      // works? lets see
    cgi_ptr->cgiOnly();  // works normally

    delete cgi;
    return 0;
}
