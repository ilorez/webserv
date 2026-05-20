#include "../../includes/container.hpp"
#include "ctime"

sessionManager &sessionManager::getInstance()
{
	static sessionManager instance;
	return instance;
};

Session *sessionManager::createSession()
{
	Session s;

	s.setId(generateSessionId());
	s.setCreatedAt(time(NULL));
	s.setLastAccess(time(NULL));
	s.setExpiresAt(time(NULL) + SESSION_TTL);
	database[s.getId()] = s;

	return &database[s.getId()];
};

std::string sessionManager::generateSessionId()
{
	std::srand((unsigned int)std::time(0));

	int d1 = std::rand() % 10;
	int d2 = std::rand() % 10;
	int d3 = std::rand() % 10;

	std::stringstream ss;
	ss << d1 << d2 << d3;
	std::string randomString = ss.str();

	return (randomString);
}

bool sessionManager::isExpired(Session &s)
{
	return time(NULL) > s.getExpiresAt();
}

Session *sessionManager::getSession(const std::string &id)
{
	std::map<std::string, Session>::iterator it = database.find(id);

	if (it == database.end())
		return NULL;

	if (isExpired(it->second))
	{
		DEBUG_INFO2("Found cookie, but expired");
		database.erase(it);
		return NULL;
	}

	it->second.setLastAccess(time(NULL));
	return &it->second;
};

void sessionManager::refrech(Session &s)
{
	s.setLastAccess(time(NULL));
	s.setExpiresAt(time(NULL) + SESSION_TTL);
}

void sessionManager::deleteSession(const std::string &id)
{
	database.erase(id);
};
sessionManager::sessionManager() {

};
sessionManager::~sessionManager() {};