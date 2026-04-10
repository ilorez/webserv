#include "../../includes/ManageClients.hpp"

ManageClients::ManageClients() {}

ManageClients::~ManageClients()
{
    std::map<int, Client*>::iterator it = _clients.begin();
    while (it != _clients.end())
    {
        delete it->second;
        ++it;
    }
    _clients.clear();
}
