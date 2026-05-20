#pragma once

class Session
{
private:
	std::string id;
	time_t createdAt;
	time_t lastAccess;
	time_t expiresAt;
	std::map<std::string, std::string> data;

public:
	void setId(const std::string &id);
	void setCreatedAt(const time_t &time);
	void setLastAccess(const time_t &time);
	void setExpiresAt(const time_t &time);
	void setData(const std::string &key, const std::string &value);

	std::string getId();
	time_t getCreatedAt();
	time_t getLastAccess();
	time_t getExpiresAt();

	Session();
	~Session();
};
