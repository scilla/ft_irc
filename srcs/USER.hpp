#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include "channel.hpp"

typedef struct s_Roles {
	bool speak;
	bool admin;
} Roles;

class User
{
	private:
		size_t 						_id;
		std::string 				_nick; //max 9 chars
		std::string 				_psw;
		std::map<Channel, Roles>	channels;
	public:
		User(size_t id, std::string nick, std::string psw);
		~User();
		std::string get_nick() const;
		std::string get_psw() const;
		void set_nick(std::string nick);
		void set_pwd(std::string psw);
		Roles						get_roles;
};

#endif