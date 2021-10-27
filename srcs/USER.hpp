#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <map>

typedef struct s_Roles {
	bool speak;
	bool admin;
} Roles;

enum USER_STATE {
	INIT,
	REGISTERED, 

};

class User
{
	private:
		size_t 					_id;
		std::string 			_nick; //max 9 chars
		std::string 			_username;
		//std::string 			_psw;
		//std::map<int, Roles>	channels;
		int						_fdread;
		int						_fdwrite;
	public:
		User(size_t id, std::string nick, std::string psw);
		~User();
		std::string get_nick() const;
		std::string get_psw() const;
		void set_nick(std::string nick);
		void set_pwd(std::string psw);
		int						get_roles;
};

User::User(size_t id, std::string nick, std::string username) : _id(id),
														   _nick(nick),
														   _username(username)
{
}

User::~User() {}

std::string User::get_nick() const
{
	return (_nick);
}


void User::set_nick(std::string nick)
{
	_nick = nick;
}

#endif