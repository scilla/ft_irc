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
		//IRC() {};
		IRC(std::string host, size_t net_pt, std::string net_psw, size_t pt, std::string psw, bool own);
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


		void parse(std::string raw);
		//void elab_parsed(std::vector<std::string> parsed);
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
		int				connected_fd;
		User*			current_user;


		char buff[500];
		int newSocket;
		void accepter();
		void handler(int connected_fd);
		void responder(std::string, int);
		std::string psw;

		std::map<size_t, User> 			USER_MAP;
		std::map<std::string, size_t> 	FD_MAP;
		std::map<std::string, Channel> 	CHANNEL_MAP;
		std::map<std::string, int(IRC::*)(std::vector<std::string>)> CMD_MAP;


		//commands
		int userCmd(std::vector<std::string>);
		int passCmd(std::vector<std::string>);
		int nickCmd(std::vector<std::string>);
		int joinCmd(std::vector<std::string>);
};

IRC::IRC(std::string host, size_t net_pt, std::string net_psw, size_t pt, std::string psw, bool own):
		_own(own),
		_host(host),
		_network_port(net_pt),
		_network_password(net_psw),
		_port(pt),
		_password(psw),
		Server(AF_INET, SOCK_STREAM, 0, pt, INADDR_ANY, 10496)
		{
			//CMD_MAP.insert(std::pair<std::string, void(IRC::*)(std::string)>("ciao", &IRC::parse));
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
/*
PASS ciao
NICK vaffa

PASS, ciao, NICK, vaffa
*/

void IRC::parse(std::string raw)
{
	std::vector<std::string>	parsed;
	std::string tmp = raw;
	int args  = 0;
	int prev_pos = 0;
	int start;
	int stop;

	start = 0;
	while (start < raw.size())
	{
		stop = raw.find(" ", start);
		if (stop == -1)
			stop = raw.size();
		if (start < stop) {
			std::cout << "found this parse: " << raw.substr(start, stop) << std::endl;
			parsed.push_back(raw.substr(start, stop));
		}
		start = stop + 1;
	}
	if (!parsed.size())
		return;
/*
	for(int i = 0; i < tmp.size() ; i++)
	{
		if(isspace(tmp[i]))
		{
			if(prev_pos != i)
				parsed.push_back(tmp.substr(prev_pos, i));
			prev_pos = i;
		}
	}
	*/

	for(std::vector<std::string>::iterator it = parsed.begin(); it != parsed.end(); it++)
	{
		std::cout << *it << " " << std::endl;
		//for(int i = 0; (*it).c_str()[i] != 0; i ++)
		//	std::cout << (int)(*it).c_str()[i] << std::endl;
	}
	current_user = get_user(connected_fd);
	std::pair<std::map<size_t, User>::iterator, bool> res;
	if (!current_user) {
		if(passCmd(parsed))
			return ;
		res = USER_MAP.insert(std::pair<size_t, User>(connected_fd, User(connected_fd)));
		current_user = &res.first.operator*().second;
	}
	if(!current_user->_state.nick)
	{
		if(!nickCmd(parsed))
			return;
	}
	if(!current_user->_state.user)
	{
		if(!userCmd(parsed))
			return;
	}

	//elab_parsed(parsed);
}

//void elab_parsed(std::vector<std::string> parsed)
//{
//	
//}

void IRC::handler(int connected_fd) {
	int start;
	int stop_r;
	int stop_n;
	int stop;
	std::string replyMessage;
	recv(connected_fd, buff, 500, 0);
	abort_connection(connected_fd);
	std::string messageFromClient(buff);
	std::cout<< ">> " << buff << " <<" << std::endl;
	start = 0;
	if(messageFromClient.size()) {
		while (start < messageFromClient.size())
		{
			stop_r = messageFromClient.find("\r", start);
			stop_n = messageFromClient.find("\n", start);
			stop = stop_r < stop_n ? stop_r : stop_n;
			if (start + 1 < stop) {
				std::cout << "found this: " << messageFromClient.substr(start, stop) << std::endl;
				parse(messageFromClient.substr(start, stop));
			}
			start = stop + 1;
		}
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
				connected_fd = *it;
				handler(connected_fd);
				break;
			}
		}
		//accepter();
		//responder();
		std::cout << "------------DONE-----------" << std::endl;

	}
}
#endif /* IRC_HPP */
