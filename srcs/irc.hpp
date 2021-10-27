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
#include <set>

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

		// bool 					is_own();
		// size_t 					get_netPort();
		// std::string 			get_netPsw();
		// size_t 					get_port();
		bool 					check_psw(std::string psw);
		User					get_user(std::string nickname);
		User					get_user(size_t fd);
		std::list<Channel>		get_channels();
		Channel					get_channel(std::string channelname);

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
		std::set<int>	readfds;
		std::set<int>	writefds;

		char buff[500];
		int newSocket;
		void accepter();
		void handler(int connected_fd);
		void responder();
		std::string psw;

		std::map<size_t, User> 			USER_MAP;
		std::map<std::string, size_t> 	FD_MAP;
		std::map<std::string, Channel> 	CHANNEL_MAP;
};

void IRC::accepter(){
}

void IRC::handler(int connected_fd){
	recv(connected_fd, buff, 500, 0);
	if(!buff[0])
	{
		close(connected_fd); std::cout << "closed connection \n";
		readfds.erase(readfds.find(connected_fd));
		return ;
	}
	std::cout<< ">> " << buff << " <<" << std::endl;
	bzero(buff, sizeof(buff));
	std::string message("433");
	write(connected_fd, message.c_str(), message.length());
	//send(connected_fd, &433, 4);
}

void IRC::responder() {
	std::string message("Hello from server");
	write(newSocket, message.c_str(), message.length());
	//close(newSocket);
}
void IRC::launch() {
	struct sockaddr_in remote = getServerSocket()->getRemote();
	int remoteLen = sizeof(remote);
	struct timeval timeout;
	timeout.tv_usec = 100;
	int max;

	fd_set fds; //fess d sort

	while(true){
		max = getServerSocket()->getSocket() + 1;
		FD_ZERO(&fds);
		FD_SET(getServerSocket()->getSocket(), &fds);
		for(std::set<int>::iterator it = readfds.begin(); it != readfds.end(); it++) {
			FD_SET(*it, &fds);
			if (*it >= max)
				max = *it + 1;
		}
		std::cout << "------------WAITING-------------" << std::endl;
		select(max, &fds, NULL, NULL, &timeout);
		if(FD_ISSET(getServerSocket()->getSocket(), &fds)){
			newSocket = accept(getServerSocket()->getSocket(), (struct sockaddr *)&remote, (socklen_t * )&remoteLen);
			std::cout << "new connection accepted\n"; 
			readfds.insert(newSocket);
		}
		for(std::set<int>::iterator it = readfds.begin(); it != readfds.end(); it++)
		{
			if(FD_ISSET(*it, &fds))
			{
				handler(*it);
				break;
			}
		}
		//accepter();
		//responder();
		std::cout << "------------DONE-----------" << std::endl;

	}
}
#endif /* IRC_HPP */
