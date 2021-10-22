#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <map>

typedef struct s_Roles {
	bool speak;
	bool admin;
} Roles;

class User
{
	private:
		size_t 					_id;
		std::string 			_nick; //max 9 chars
		std::string 			_psw;
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

User::User(size_t id, std::string nick, std::string psw) : _id(id),
														   _nick(nick),
														   _psw(psw)
{
}

User::~User() {}

std::string User::get_nick() const
{
	return (_nick);
}

std::string User::get_psw() const
{
	return (_psw);
}

void User::set_nick(std::string nick)
{
	_nick = nick;
}

void User::set_pwd(std::string psw)
{
	_psw = psw;
}

#endif