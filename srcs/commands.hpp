#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include "irc.hpp"
#include "USER.hpp"

int IRC::initializer(std::vector<std::string> parsed)
{
	char str[INET_ADDRSTRLEN];

	std::vector<std::string> str_vect;
	current_user = get_user(connected_fd);
	std::pair<std::map<size_t, User>::iterator, bool> res;
	if (!current_user) {
		if(passCmd(parsed))
			return 1;
		inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
		str_vect = splitter(str, '.');
		res = USER_MAP.insert(std::pair<size_t, User>(connected_fd, User(connected_fd, str_vect[0] + "." + str_vect[1] + "." + str_vect[2], str_vect[3])));
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

	current_user = get_user(connected_fd);
	parsed = splitter(raw, ' ');
	command = parsed[0];
	params = raw.substr(command.size() + 1, raw.size());
	if(!command.compare("PING"))
		pongCmd(params);
	else if(!command.compare("JOIN"))
		joinCmd(params);
	else if(!command.compare("NICK"))
		nickCmd(parsed);
	else if(!command.compare("QUIT"))
		quitCmd(params);
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
		responder(ERR_NEEDMOREPARAMS, *current_user);
		abort_connection(connected_fd);
		return (1);
	}
	if(parsed[1].compare(":" + this->_password) && parsed[1].compare(this->_password))
	{
		std::cout<< "User sent wrong pass" << std::endl;
		responder(ERR_PASSWDMISMATCH, *current_user);
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
		responder(ERR_NONICKNAMEGIVEN, *current_user);
		return 1;
	}
	if(parsed[1].size() > 9){
		responder(ERR_ERRONEUSNICKNAME, *current_user);
		return 1;
	}
	for(std::map<size_t, User>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++)
	{
		if(!parsed[1].compare(it.operator*().second.get_nick())) {
			responder(ERR_NICKNAMEINUSE, *current_user);
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
		responder(ERR_NEEDMOREPARAMS, *current_user);
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
	responder(message, *current_user);
	return 0;
}

int IRC::joinCmd(std::string raw)
{
	std::vector<std::string> 	params = splitter(raw, ' ');
	std::vector<std::string> 	channels;
	std::vector<std::string> 	keys;
	Channel* 					current_channel;

	if(!params.size())
	{
		responder(ERR_NEEDMOREPARAMS, *current_user);
		return 1;
	}
	std::cout << raw << " RAW" << std::endl;
	std::cout << params[0] << " PARAMS" << std::endl;
	channels = splitter(params[0], ',');
	if (params.size() > 1)
		keys = splitter(params[1], ',');
	for (int i = 0; i < channels.size(); i++) {
		if (channels[i][0] != '#') {
			responder(ERR_NOSUCHCHANNEL, *current_user);
			continue;
		}
		current_channel = &get_channel(channels[i]);
		if (i < keys.size()) {
			current_channel->userJoin(*current_user, keys[i]);
		} else {
			current_channel->userJoin(*current_user);
		}
	}
	return 0;
}

int IRC::quitCmd(std::string raw)
{
	std::vector<std::string> params = splitter(raw, ' ');
	if(params.size())
		responder(current_user->get_nick(), current_user);
	else
		responder(params[0], current_user);
	//rimuovere utente da canali
	readfds.erase(readfds.find(current_user->get_id()));
	USER_MAP.erase(current_user->get_id());
	


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
	return 1;
}

#endif /*COMMAND_HPP*/

