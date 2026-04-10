#ifndef MANAGECLIENTS_HPP
#define MANAGECLIENTS_HPP

#include <map>
#include "Client.hpp"

class ManageClients
{
    private:
        std::map<int, Client*> _clients;
    public:
        ManageClients();
        ~ManageClients();

        void    addClient(int fd);
        void    removeClient(int fd);
        Client* getClient(int fd);
        bool    clientExists(int fd) const;
    private:
        ManageClients(const ManageClients& other);
        ManageClients& operator=(const ManageClients& other);


};

#endif
