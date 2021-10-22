#include "user.hpp"

User::User(size_t id, std::string nick, std::string psw): 
_id(id),
_nick(nick),
_psw(psw)
{}

User::~User() {}

std::string User::get_nick() {
	return(_nick);
}

std::string User::get_psw() {
	return(_psw);
}

void User::set_nick(std::string nick) {
	_nick = nick;
}

void User::set_pwd(std::string psw) {
	_psw = psw;
}

