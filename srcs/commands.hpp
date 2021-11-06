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

void	IRC::commandSelector(std::string raw)
{
	std::vector<std::string>	parsed;
	std::string					command;
	std::string					params;

	parsed = splitter(raw, ' ');
	command = parsed[0];
	params = raw.substr(parsed.size() + 1, raw.size());
	if(command.compare("PONG"))
		pongCmd(params);
	else if(command.compare("JOIN"))
		joinCmd(params);
	else if(command.compare("NICK"))
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

	if ((!parsed[0].compare("PASS") && parsed.size() < 2) || (parsed[0].compare("PASS"))){
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

int IRC::pongCmd(std::string raw)
{
	std::vector<std::string> parsed = splitter(raw, ' ');
	std::string message("PONG ");

	message.append(parsed[2] + " ");
	message.append(parsed[1] + "\n");
	responder(message, connected_fd);
	return 0;
}

int IRC::joinCmd(std::string raw)
{
	std::map<std::string, Channel>::iterator res;
	if(params.size() > 1)
	{
		if(params[0].compare(params[0].size(), 1, "#")) //channel mode
		{
	return 0;
}

/*4.2.3.1 Channel modes

   Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>]
               [<ban mask>]

   The MODE command is provided so that channel operators may change the
   characteristics of `their' channel.  It is also required that servers
   be able to change channel modes so that channel operators may be
   created.

   The various modes available for channels are as follows:

           o - give/take channel operator privileges;
           p - private channel flag;
           s - secret channel flag;
           i - invite-only channel flag;
           t - topic settable by channel operator only flag;
           n - no messages to channel from clients on the outside;
           m - moderated channel;
           l - set the user limit to channel;
           b - set a ban mask to keep users out;
           v - give/take the ability to speak on a moderated channel;
           k - set a channel key (password).

   When using the 'o' and 'b' options, a restriction on a total of three
   per mode command has been imposed.  That is, any combination of 'o'
   and

4.2.3.2 User modes

   Parameters: <nickname> {[+|-]|i|w|s|o}

   The user MODEs are typically changes which affect either how the
   client is seen by others or what 'extra' messages the client is sent.
   A user MODE command may only be accepted if both the sender of the
   message and the nickname given as a parameter are both the same.

   The available modes are as follows:

           i - marks a users as invisible;
           s - marks a user for receipt of server notices;
           w - user receives wallops;
           o - operator flag.

   Additional modes may be available later on.

   If a user attempts to make themselves an operator using the "+o"
   flag, the attempt should be ignored.  There is no restriction,
   however, on anyone `deopping' themselves (using "-o").  Numeric
   Replies:

           ERR_NEEDMOREPARAMS              RPL_CHANNELMODEIS
           ERR_CHANOPRIVSNEEDED            ERR_NOSUCHNICK
           ERR_NOTONCHANNEL                ERR_KEYSET
           RPL_BANLIST                     RPL_ENDOFBANLIST
           ERR_UNKNOWNMODE                 ERR_NOSUCHCHANNEL

           ERR_USERSDONTMATCH              RPL_UMODEIS
           ERR_UMODEUNKNOWNFLAG

   Examples:

           Use of Channel Modes:

MODE #Finnish +im               ; Makes #Finnish channel moderated and
                                'invite-only'.

MODE #Finnish +o Kilroy         ; Gives 'chanop' privileges to Kilroy on
*/

int IRC::modeCmd(std::string raw)
{
	std::vector<std::string> params;
	params = splitter(raw, ' ');
	if(params.size() > 1)
	{
		if(params[0].compare(params[0].size(), 1, "#")) //channel mode
		{
			std::map<std::string, Channel>::iterator res;
			if((res = CHANNEL_MAP.find(params[0].substr(1, params[0].size()))) != CHANNEL_MAP.end()) // canale trovato
			{

			}
		}
		else //user mode
		{

		}
	}
	//error ERR_NEEDMOREPARAMS 
}

#endif /*COMMAND_HPP*/

