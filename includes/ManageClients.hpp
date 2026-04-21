#ifndef MANAGECLIENTS_HPP
#define MANAGECLIENTS_HPP

#include <map>
#include "Client.hpp"

class ManageClients
{
    private:
        std::map<int, Client*> _clients;
        int _epfd;
    public:
        ManageClients();
        ~ManageClients();

        void    addClient(int fd);
        Client* getClient(int fd);
        bool    clientExists(int fd) const;
        void    setEpfd(int fd);
        void    checkTimeout();
        void    disconnect(int fd);
        void    updateToCGI(int fd);
    private:
        ManageClients(const ManageClients& other);
        ManageClients& operator=(const ManageClients& other);


};

#endif
