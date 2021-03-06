#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <strings.h>
#include <set>
#include <fstream>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

struct in_addr ipAddr;
#include "USER.hpp"
#include "CHANNEL.hpp"
#include "netinet/in.h"
#include "networking/server/Server.hpp"
#include "networking/socket/ConnectingSocket.hpp"
#include "errors.hpp"
#include "utils.hpp"

class IRC : public Server
{
public:
	IRC(std::string host, size_t net_pt, std::string net_psw, size_t pt, std::string psw, bool own);
	~IRC(){};
	bool check_psw(std::string psw);
	User *get_user(std::string nickname);
	User *get_user(int fd);
	bool is_user(int fd);
	std::list<Channel> get_channels();
	Channel &get_channel(std::string channelname);
	ConnectingSocket *remoteServer;
	void launch();
	void check_connection(void);
	int initializer(std::vector<std::string> parsed);
	void commandSelector(std::string parsed);
	void user_creator();
	void user_logged();
	void parse(std::string raw);

private:
	bool _own;
	std::string _host;
	size_t _network_port;
	std::string _network_password;
	size_t _port;		   //local server port
	std::string _password; //local server psw
	std::string own_ip;
	std::set<int> readfds;
	std::set<int> writefds;
	size_t connected_fd;
	User *current_user;
	struct sockaddr_in remote;
	char hostname[_SC_HOST_NAME_MAX];
	char buff[300000];
	int newSocket;
	void accepter();
	void handler(int connected_fd);
	std::string receiver();
	std::string psw;
	std::map<size_t, User> USER_MAP;
	std::map<std::string, size_t> FD_MAP;
	std::map<std::string, Channel *> CHANNEL_MAP;
	std::map<std::string, int (IRC::*)(std::vector<std::string>)> CMD_MAP;
	User &nick_to_user(std::string nick);
	//commands
	int userCmd(std::vector<std::string>);
	int passCmd(std::vector<std::string>);
	int nickCmd(std::vector<std::string>);
	int joinCmd(std::string);
	int partCmd(std::string);
	int pongCmd(std::string);
	int modeCmd(std::string);
	int quitCmd(std::string);
	int privmsgCmd(std::string);
	int whoCmd(std::string);
	int namesCmd(Channel);
	int listCmd(std::string);
	int topicCmd(std::string);
	int motdCmd(std::string);
	int noticeCmd(std::string);
	void inviteCmd(std::string);
	int kickCmd(std::string);

};

IRC::IRC(std::string host, size_t net_pt, std::string net_psw, size_t pt, std::string psw, bool own) : 
	Server(AF_INET, SOCK_STREAM, 0, pt, INADDR_ANY, 10496),
	_own(own),
	_host(host),
	_network_port(net_pt),
	_network_password(net_psw),
	_port(pt),
	_password(psw)
{
	current_user = NULL;
};

void IRC::accepter()
{
}

Channel &IRC::get_channel(std::string channel_name)
{
	std::map<std::string, Channel *>::iterator it;
	Channel *res;

	it = CHANNEL_MAP.find(channel_name);
	if (it == CHANNEL_MAP.end())
	{
		res = new Channel(channel_name);
		CHANNEL_MAP.insert(std::pair<std::string, Channel *>(channel_name, res));
		std::cout << "NUOVOCANALE CREATO: " << channel_name << std::endl;
		return *res;
	}
	else
	{
		std::cout << "CANALE TROVATO: " << it.operator*().first << std::endl;
		return *(it.operator*().second);
	}
}

bool IRC::check_psw(std::string psw)
{
	std::cout << "\nsent: " << psw << "|\npass: " << _password << "|" << std::endl;
	return ((psw == _password) ? true : false);
}

User *IRC::get_user(int fd)
{
	std::map<size_t, User>::iterator found;

	found = USER_MAP.find(fd);
	if (found != USER_MAP.end())
		return (&found.operator*().second);
	return (NULL);
}

User *IRC::get_user(std::string nickname)
{
	std::map<size_t, User>::iterator found = USER_MAP.begin();

	for (; found != USER_MAP.end(); found++)
		if (found->second.get_nick() == nickname)
			break;
	if (found != USER_MAP.end())
		return (&found->second);
	return (NULL);
}

void IRC::check_connection(void)
{
	if (!buff[0])
	{
		for (std::map<size_t, User>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++)
		{
			if (connected_fd == (*it).second.get_id())
			{
				quitCmd("");
				return;
			}
		}
		close(connected_fd);
		readfds.erase(connected_fd);
	}
	return;
}

void IRC::user_logged()
{
	char str[100];
	std::string message(RPL_WELCOME);
	message.append(" ");
	message.append(current_user->get_nick());
	message.append(" :Hi welcome to IRC");
	responder(message, *current_user);
	message.clear();
	message.append(RPL_YOURHOST);
	message.append(" ");
	message.append(current_user->get_nick());
	message.append(" :Your host is ");
	message.append(inet_ntop(AF_INET, &remote.sin_addr.s_addr, str, INET_ADDRSTRLEN));
	message.append(", running version ft_irc-0.1");
	responder(message, *current_user);
	message.clear();
	message.append(RPL_CREATED);
	message.append(" ");
	message.append(current_user->get_nick());
	message.append(" :This server was created sometime");
	responder(message, *current_user);
	message.clear();
	message.append(RPL_MYINFO);
	message.append(" ");
	message.append(current_user->get_nick());
	message.append(" ");
	message.append(inet_ntop(AF_INET, &remote.sin_addr.s_addr, str, INET_ADDRSTRLEN));
	message.append(" ft_irc-0.1 o o");
	responder(message, *current_user);
	message.clear();
	message.append(RPL_LUSERCLIENT);
	message.append(" ");
	message.append(current_user->get_nick());
	message.append(": there are ");
	std::stringstream tmp;
	tmp << USER_MAP.size();
	message.append(tmp.str());
	message.append(" users online, *services and servers to be done*");
	responder(message, *current_user);
	motdCmd("");
}

std::vector<std::string> splitter(std::string raw, char sep)
{
	std::vector<std::string> parsed;
	std::string token;
	std::stringstream tokenStream(raw);

	while (std::getline(tokenStream, token, sep))
		parsed.push_back(token);
	if (!parsed.size())
		parsed.push_back("");
	return parsed;
}

void IRC::parse(std::string raw)
{
	std::vector<std::string> parsed;
	std::string command;
	std::string tmp = raw;
	int start;

	start = 0;
	std::vector<std::string> tokens;
	std::string token;
	std::stringstream tokenStream(raw);
	parsed = splitter(raw, ' ');
	if (!(parsed.size()))
		return;
	int res;
	if ((res = initializer(parsed)) == 1)
		return;
	if (!res)
		commandSelector(raw);
}

void IRC::handler(int connected_fd)
{
	std::string messageFromClient = receiver();
	std::string token;
	replace_substr(messageFromClient, "\r\n");
	std::stringstream tokenStream(messageFromClient);
	while (std::getline(tokenStream, token))
	{
		parse(token);
	}
	bzero(buff, sizeof(buff));
	(void)connected_fd;
}

std::string IRC::receiver()
{
	recv(connected_fd, buff, 500, 0);
	check_connection();
	std::string messageFromClient(buff);
	if (current_user)
		print_prompt(1, current_user->get_remote_ip(), messageFromClient);
	return (messageFromClient);
}

void IRC::launch()
{
	signal(SIGPIPE, SIG_IGN);
	char str[INET_ADDRSTRLEN];
	current_user = NULL;
	remote = getServerSocket()->getRemote();
	int remoteLen = sizeof(remote);
	ipAddr = remote.sin_addr;
	inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
	this->own_ip = str;
	struct timeval timeout;
	timeout.tv_usec = 100;
	int max;

	fd_set fds; //fess d sort

	while (true)
	{
		max = getServerSocket()->getSocket() + 1;
		FD_ZERO(&fds);
		FD_SET(getServerSocket()->getSocket(), &fds);
		for (std::set<int>::iterator it = readfds.begin(); it != readfds.end(); it++)
		{
			FD_SET(*it, &fds);
			if (*it >= max)
				max = *it + 1;
		}
		select(max, &fds, NULL, NULL, &timeout);
		if (FD_ISSET(getServerSocket()->getSocket(), &fds))
		{
			newSocket = accept(getServerSocket()->getSocket(), (struct sockaddr *)&remote, (socklen_t *)&remoteLen);
			std::cout << "new connection accepted\n";
			readfds.insert(newSocket);
		}
		for (std::set<int>::iterator it = readfds.begin(); it != readfds.end(); it++)
		{
			if (FD_ISSET(*it, &fds))
			{
				connected_fd = *it;
				if (USER_MAP.count(connected_fd))
					current_user = get_user(connected_fd);
				handler(connected_fd);
				break;
			}
		}
	}
}
#endif /* IRC_HPP */
