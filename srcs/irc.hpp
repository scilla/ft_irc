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

#define ERR_NONICKNAMEGIVEN		"431\n"
#define ERR_ERRONEUSNICKNAME	"432\n"
#define ERR_NICKNAMEINUSE		"433\n"
// #define ERR_NICKCOLLISION		"436\n"
#define ERR_NOLOGIN				"444\n"
#define ERR_NEEDMOREPARAMS		"461\n"
#define ERR_PASSWDMISMATCH		"464\n"

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
		User*					get_user(std::string nickname);
		User*					get_user(int fd);
		bool					is_user(int fd);
		std::list<Channel>		get_channels();
		Channel					get_channel(std::string channelname);

		void					launch();
		void					abort_connection(int disconnected_fd);
		void					user_creator();
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
		void responder(std::string, int);
		std::string psw;

		std::map<size_t, User> 			USER_MAP;
		std::map<std::string, size_t> 	FD_MAP;
		std::map<std::string, Channel> 	CHANNEL_MAP;
};

void IRC::accepter(){
}

bool IRC::check_psw(std::string psw) {
	for (int i = 0; i < psw.length(); i++) {
		printf("%d ", psw[i]);
	}
	std::cout << "\nsent: " << psw << "|\npass: " << _password << "|" << std::endl;
	for (int i = 0; i < _password.length(); i++) {
		printf("%d ", _password[i]);
	}
	printf("\n");
	return ((psw == _password) ? true : false);
}

User* IRC::get_user(int fd) {
	std::map<size_t, User>::iterator found;

	found = USER_MAP.find(fd);
	if(found != USER_MAP.end())
		return(&found.operator*().second);
	return(NULL);
}

void	IRC::abort_connection(int disconnected_fd){

	if(!buff[0])
	{
		close(disconnected_fd); std::cout << "closed connection \n";
		readfds.erase(readfds.find(disconnected_fd));
		return ;
	}
}



void IRC::handler(int connected_fd) {
	std::string replyMessage;
	User*	current_user = get_user(connected_fd);
	recv(connected_fd, buff, 500, 0);
	abort_connection(connected_fd);
	std::string messageFromClient(buff);
	std::cout<< ">> " << buff << " <<" << std::endl;
	if (!current_user) {
		if (messageFromClient.substr(0, 6) != "PASS :") {
			std::cout<< "User sent no pass" << std::endl;
			responder(ERR_NEEDMOREPARAMS, connected_fd);
			return;
		}
		if (!check_psw(messageFromClient.substr(6, messageFromClient.find("\x0d") - 6))) {
			std::cout<< "User sent wrong pass" << std::endl;
			bzero(buff, sizeof(buff));
			abort_connection(connected_fd);
			return;
		}
		USER_MAP.insert(std::pair<size_t, User>(connected_fd, User(connected_fd)));
	}
	bzero(buff, sizeof(buff));
}

void IRC::responder(std::string message, int fd) {
	write(fd, message.c_str(), message.length());
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
