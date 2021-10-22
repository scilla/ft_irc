#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <list>
#include <set>
#include "user.hpp"

class Channel
{
	private:
		size_t					_id;
		std::string				_name;
		std::string				psw;
		std::map<size_t, User>	USE_RGH; //mappa con user right come key e utente corrispondende
		std::set<User>			banned_users;
	public:
		Channel();
		~Channel();

};

#endif // CHANNEL_HPP