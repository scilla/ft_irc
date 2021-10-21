#include "USER.hpp"

USER::USER(size_t id, std::string nick, std::string psw): 
_id(id),
_nick(nick),
_psw(psw)
{}

USER::~USER() {}

std::string USER::get_nick() {
	return(_nick);
}

std::string USER::get_psw() {
	return(_psw);
}

void USER::set_nick(std::string nick) {
	_nick = nick;
}

void USER::set_pwd(std::string psw) {
	_psw = psw;
}

