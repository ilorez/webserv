#include "../../includes/container.hpp"

void Session::setId(const std::string &id) { this->id = id; };
void Session::setCreatedAt(const time_t &time) { this->createdAt = time; }
void Session::setExpiresAt(const time_t &time) { this->expiresAt = time; }
void Session::setData(const std::string &key, const std::string &value) { data[key] = value; };

std::string Session::getId() { return this->id; };
time_t Session::getCreatedAt() { return this->createdAt; };
time_t Session::getExpiresAt() { return this->expiresAt; };

Session::Session() {};
Session::~Session() {};

// set-Cookies: