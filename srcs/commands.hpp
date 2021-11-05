#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include "irc.hpp"
#include "USER.hpp"

int IRC::initializer(std::vector<std::string> parsed)
{
	current_user = get_user(connected_fd);
	std::pair<std::map<size_t, User>::iterator, bool> res;
	if (!current_user) {
		if(passCmd(parsed))
			return 1;
		res = USER_MAP.insert(std::pair<size_t, User>(connected_fd, User(connected_fd)));
		current_user = &res.first.operator*().second;
		return 1;
	}
	if(!current_user->_state.nick)
	{
		if(!parsed[0].compare("NICK"))
			nickCmd(parsed);
		return 1;
	}
	if(!current_user->_state.user)
	{
		if(!parsed[0].compare("USER"))
		{
			if(!userCmd(parsed))
				user_logged();
		}
		return 1;
	}
	return 0;
}

void	IRC::commandSelector(std::vector<std::string> parsed)
{
	if(parsed[0].compare("PONG"))
		pongCmd(parsed);
	else if(parsed[0].compare("JOIN"))
		joinCmd(parsed);
	else if(parsed[0].compare("NICK"))
		nickCmd(parsed);
	//else if(parsed[0].compare("PRIVMSG"))
	//	privmsgCmd(parsed);
	//else if(parsed[0].compare("KILL"))
	//	killCmd(parsed);
	//else if(parsed[0].compare("WHO"))
	//	whoCmd(parsed);


}

int IRC::passCmd(std::vector<std::string> parsed)
{
	if (!parsed[0].compare("PASS") && parsed.size() < 2) {
		std::cout<< "User sent no pass" << std::endl;
		responder(ERR_NEEDMOREPARAMS, connected_fd);
		abort_connection(connected_fd);
		return (1);
	}
	if(parsed[1].compare(":" + this->_password) && parsed[1].compare(this->_password))
	{
		std::cout<< "User sent wrong pass" << std::endl;
		responder(ERR_PASSWDMISMATCH, connected_fd);
		bzero(buff, sizeof(buff));
		abort_connection(connected_fd);
		return (1);
	}
	return 0;
}

int IRC::nickCmd(std::vector<std::string> parsed)
{

	std::cout<< "USER SENT NICK" << std::endl;
	if(parsed.size() < 2){
		responder(ERR_NONICKNAMEGIVEN, connected_fd);
		return 1;
	}
	if(parsed[1].size() > 9){
		responder(ERR_ERRONEUSNICKNAME, connected_fd);
		return 1;
	}
	for(std::map<size_t, User>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++)
	{
		if(!parsed[1].compare(it.operator*().second.get_nick())) {
			responder(ERR_NICKNAMEINUSE, connected_fd);
			return 1;
		}
	}
	current_user->set_nick(parsed[1]);
	return 0;
}

int IRC::userCmd(std::vector<std::string> parsed)
{
	std::cout<< "USER SENT USER" << std::endl;
	if(parsed.size() < 2){
		responder(ERR_NEEDMOREPARAMS, connected_fd);
		return 1;
	}
	current_user->set_username(parsed[1]);
	return 0;
}

int IRC::pongCmd(std::vector<std::string> parsed)
{
	std::string message("PONG ");

	message.append(parsed[2] + " ");
	message.append(parsed[1] + "\n");
	responder(message, connected_fd);
	return 0;
}

int IRC::joinCmd(std::vector<std::string> parsed)
{
	return 0;
}
#endif /*COMMAND_HPP*/