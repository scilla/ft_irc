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
	bool invisible;
	bool server_notices;
	bool wallops; //non lo implementiamo
	bool op;
	
}				t_state;


class User
{
	private:
		const size_t 			_id;
		std::string 			_nick; //max 9 chars
		std::string				_altnick;
		std::string 			_username;
		std::string				_realname;
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
		std::string get_realname() const;
		std::string get_full_address() const;
		std::string get_identifier() const;
		std::string get_ip_str();
		size_t		get_user_nb();
		size_t 		get_id() const;
		std::string get_altnick() const;
		void set_nick(std::string nick);
		void set_username(std::string nick);
		void set_host(std::string host);
		void set_domain(std::string domain);
		int						get_roles;
		t_state					_state;
};

User::User(size_t id, std::string host, std::string domain) : _id(id), _state((t_state){true, false, false, false, false, false, false}), _host(host), _domain(domain)
{
	std::cout << "New user created, fd: " << id << std::endl;
}

User::~User() {}

std::string User::get_nick() const
{
	return (_nick);
}

std::string User::get_realname() const{
	return _realname;
}

std::string User::get_altnick() const
{
	return _altnick;
}



std::string User::get_username() const{
	return _username;
}

std::string User::get_ip_str() {
	std::string str(_domain);
	str.append(".");
	str.append(_host);
	return str;
}

size_t User::get_id() const { return _id; }

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

void User::set_host(std::string new_host) {
	_host = new_host;
}

void User::set_domain(std::string new_domain) {
	_domain = new_domain;
}

std::string User::get_host() const
{
	return _host;
}

std::string User::get_domain() const
{
	return _domain;
}

std::string User::get_full_address() const {
	return _domain + "." + _host;
}

std::string User::get_identifier() const {
	return _nick + "!" + _username + "@" + get_full_address();
}

#endif /* USER_HPP */
