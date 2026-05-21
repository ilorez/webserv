#pragma once

#include <ctime>
#include <map>
#include <string>

class Session
{
private:
	std::string id;
	time_t createdAt;
	time_t expiresAt;
	std::map<std::string, std::string> data;
public:
	void setId(const std::string &id);
	void setCreatedAt(const time_t &time);
	void setExpiresAt(const time_t &time);
	void setData(const std::string &key, const std::string &value);

	std::string getId();
	time_t getCreatedAt();
	time_t getExpiresAt();
	std::map<std::string, std::string> &getData();
	Session();
	~Session();
};
