#pragma once

class sessionManager
{
private:
	std::map<std::string, Session> database;

public:
	static sessionManager &getInstance();
	// getters
	std::map<std::string, Session> &getDatabase();
	Session *getSession(const std::string &id);
	// helpers
	std::string generateSessionId();
	Session *createSession();
	void deleteSession(const std::string &id);
	bool isExpired(Session &s);

private:
	sessionManager();
	~sessionManager();
};