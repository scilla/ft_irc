#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <map>
#include <list>
#include "USER.hpp"
#include "CHANNEL.hpp"

class IRC
{
	public:
		IRC(std::string host, size_t net_pt, std::string net_psw, size_t pt, std::string psw, bool own):
		_host(host),
		_network_port(net_pt),
		_network_password(net_psw),
		_port(pt),
		_password(psw),
		_own(own)
		{};
		~IRC() {};

		bool is_own();
		std::string get_host();
		size_t get_netPort();
		std::string get_netPsw();
		size_t get_port();
		std::string get_psw();
		std::map<size_t, USER> get_users();
		std::list<CHANNEL> get_channels();


	private:
		bool			_own;
		std::string		_host;
		size_t			_network_port;
		std::string		_network_password;
		size_t			_port;
		std::string		_password;

		std::map<size_t, USER> USER_MAP;
		std::map<std::string, CHANNEL> CHANNEL_MAP;
};


#endif