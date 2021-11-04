#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include "irc.hpp"
#include "USER.hpp"


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
	if(!parsed[0].compare("NICK"))
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
	return 1;
}

int IRC::userCmd(std::vector<std::string> parsed)
{
	if(!parsed[0].compare("USER"))
	{
		std::cout<< "USER SENT USER" << std::endl;
		if(parsed.size() < 2){
			responder(ERR_NEEDMOREPARAMS, connected_fd);
			return 1;
		}
		current_user->set_username(parsed[1]);
		return 0;
	}
	return 1;
}


#endif /*COMMAND_HPP*/