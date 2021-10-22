#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <map>
#include <list>
#include "user.hpp"
#include "channel.hpp"
#include <sys/select.h>
#include <sys/socket.h>

class IRC
{
	public:
		IRC();
		IRC(std::string host, size_t net_pt, std::string net_psw, size_t pt, std::string psw, bool own):
		_host(host),
		_network_port(net_pt),
		_network_password(net_psw),
		_port(pt),
		_password(psw),
		_own(own)
		{};
		~IRC() {};

		bool 					is_own();
		size_t 					get_netPort();
		std::string 			get_netPsw();
		size_t 					get_port();
		std::string 			get_psw();
		std::map<size_t, User>	get_users();
		std::list<Channel>		get_channels();

		void					start(std::string, std::string, std::string, std::string, std::string);
	private:
		bool			_own;
		std::string		_host;
		size_t			_network_port;
		std::string		_network_password;
		size_t			_port;
		std::string		_password;
		fd_set 			readfds;
		fd_set 			writefds;

		std::map<size_t, User> 			USER_MAP;
		std::map<std::string, Channel> 	CHANNEL_MAP;
};

std::string IRC::get_psw() {
	return this->_password;
}

void IRC::start(std::string port, std::string pass, std::string remotehost = "", std::string remoteport = "", std::string remotepass = "") {
	int	s_fd;
	std::string hostnet, pass, port, remoteport, remotepass;

	s_fd = socket(AF_INET, SOCK_STREAM, 0);
	
}
#endif /* IRC_HPP */
