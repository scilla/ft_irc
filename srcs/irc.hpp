#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <map>
#include <list>
#include "USER.hpp"
#include <sys/types.h>
#include "CHANNEL.hpp"
#include "netinet/in.h"
#include <sys/select.h>
#include <sys/socket.h>
#include "networking/server/Server.hpp"
#include <unistd.h>
#include <vector>

class IRC: public Server
{
	public:
		IRC() {};
		IRC(std::string host, size_t net_pt, std::string net_psw, size_t pt, std::string psw, bool own):
		_own(own),
		_host(host),
		_network_port(net_pt),
		_network_password(net_psw),
		_port(pt),
		_password(psw),
		Server(AF_INET, SOCK_STREAM, 0, pt, INADDR_ANY, 10496)
		{};
		~IRC() {};

		bool 					is_own();
		size_t 					get_netPort();
		std::string 			get_netPsw();
		size_t 					get_port();
		std::string 			get_psw();
		std::map<size_t, User>	get_users();
		std::list<Channel>		get_channels();

		void					launch();
	private:
		bool			_own;
		std::string		_host;
		size_t			_network_port;
		std::string		_network_password;		
		size_t			_port;					//local server port
		std::string		_password;				//local server psw
		// fd_set 			*readfds;
		// fd_set 			*writefds;
		std::vector<int>	readfds;
		std::vector<int>	writefds;

		char buff[500];
		int newSocket;
		void accepter();
		void handler();
		void responder();
		std::string psw;

		std::map<size_t, User> 			USER_MAP;
		std::map<std::string, Channel> 	CHANNEL_MAP;
};

std::string IRC::get_psw() {
	return this->_password;
}

void IRC::accepter(){
	struct sockaddr_in remote = getServerSocket()->getRemote();
	int remoteLen = sizeof(remote);
	newSocket = accept(getServerSocket()->getSocket(), (struct sockaddr *)&remote, (socklen_t * )&remoteLen);
	read(newSocket, buff, sizeof(buff));

}

void IRC::handler(){
	std::cout<< buff << std::endl;
}

void IRC::responder() {
	std::string message("Hello from server");
	write(newSocket, message.c_str(), message.length());
	//close(newSocket);
}
void IRC::launch() {
	while(true){
		std::cout << "------------WAITING-------------" << std::endl;
		accepter();
		handler();
		responder();
		std::cout << "------------DONE-----------" << std::endl;

	}
}
#endif /* IRC_HPP */
