#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <map>

typedef struct s_Roles {
	bool speak;
	bool admin;
} Roles;

typedef struct s_state{
	bool pass;
	bool nick;
	bool user;
}				t_state;


class User
{
	private:
		const size_t 			_id;
		std::string 			_nick; //max 9 chars
		std::string 			_username;
		//std::map<int, Roles>	channels;
		int						_fdread;
		int						_fdwrite;
		int						_last_activity;
	public:
		User(size_t id);
		~User();
		std::string get_nick() const;
		std::string get_username() const;
		std::string get_psw() const;
		void set_nick(std::string nick);
		void set_username(std::string nick);
		int						get_roles;
		t_state					_state;
};

User::User(size_t id) : _id(id), _state((t_state){true, false, false})
{
	std::cout << "New user created, fd: " << id << std::endl;
}

User::~User() {}

std::string User::get_nick() const
{
	return (_nick);
}


void User::set_nick(std::string nick)
{
	_state.nick = true;
	_nick = nick;
}

void User::set_username(std::string user)
{
	_state.user = true;
	_username = user;
}

#endif