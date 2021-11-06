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
		std::string				_host;
		std::string				_domain;
	public:
		User(size_t, std::string, std::string);
		~User();
		std::string get_nick() const;
		std::string get_username() const;
		std::string get_psw() const;
		std::string get_host() const;
		std::string get_domain() const;
		std::string get_ip_str();
		size_t get_id() const;
		void set_nick(std::string nick);
		void set_username(std::string nick);
		int						get_roles;
		t_state					_state;
};

User::User(size_t id, std::string host, std::string domain) : _id(id), _state((t_state){true, false, false}), _host(host), _domain(domain)
{
	std::cout << "New user created, fd: " << id << std::endl;
}

User::~User() {}

std::string User::get_nick() const
{
	return (_nick);
}

std::string User::get_username() const{
	return _username;
}

std::string User::get_ip_str() {
	std::string str = _domain + "." + _host;
	return str;
}

size_t User::get_id() const { return _id;}

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

std::string User::get_host() const
{
	return _host;
}

std::string User::get_domain() const
{
	return _domain;
}

#endif /* USER_HPP */
