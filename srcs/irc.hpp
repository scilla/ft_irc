#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>

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

	private:
		bool			_own;
		std::string		_host;
		size_t			_network_port;
		std::string		_network_password;
		size_t			_port;
		std::string		_password;
};


#endif