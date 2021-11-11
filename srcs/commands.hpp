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
	if (!current_user)
	{
		if (passCmd(parsed))
			return 1;
		str_vect = splitter(str, '.');
		res = USER_MAP.insert(std::pair<size_t, User>(connected_fd, User(connected_fd)));
		current_user = &res.first.operator*().second;
		current_user->set_remote_ip(inet_ntoa(remote.sin_addr));
		return 1;
	}
	if (!current_user->_state.nick || !current_user->_state.user)
	{
		if (!parsed[0].compare("NICK"))
		{
			nickCmd(parsed);
		}
		else if (!parsed[0].compare("USER"))
		{
			userCmd(parsed);
		}
		if (!current_user->_state.nick || !current_user->_state.user)
			return 1;
		user_logged();
	}
	return 0;
}

void IRC::commandSelector(std::string raw)
{
	std::vector<std::string> parsed;
	std::string command;
	std::string params = "";

	current_user = get_user(connected_fd);
	parsed = splitter(raw, ' ');
	command = parsed[0];
	if (parsed.size() > 1 && parsed[1].size())
		params = raw.substr(command.size() + 1, raw.size());
	if (!command.compare("PING"))
		pongCmd(params);
	else if (!command.compare("JOIN") || !command.compare("join"))
		joinCmd(params);
	else if (!command.compare("NICK"))
		nickCmd(parsed);
	else if (!command.compare("QUIT"))
		quitCmd(params);
	else if (!parsed[0].compare("PRIVMSG"))
		privmsgCmd(params);
	else if (!parsed[0].compare("PART"))
		partCmd(params);
	else if (!parsed[0].compare("LIST"))
		listCmd(params);
	else if (!parsed[0].compare("WHO"))
		whoCmd(params);
	else if (!parsed[0].compare("TOPIC"))
		topicCmd(params);
}

int IRC::passCmd(std::vector<std::string> parsed)
{
	if ((!parsed[0].compare("PASS") && parsed.size() < 2) || (parsed[0].compare("PASS")))
	{
		std::cout << "User sent no pass" << std::endl;
		responder(ERR_NEEDMOREPARAMS, connected_fd);
		// abort_connection(connected_fd);
		close(connected_fd);
		readfds.erase(connected_fd);
		return (1);
	}
	if (parsed[1].compare(":" + this->_password) && parsed[1].compare(this->_password))
	{
		std::cout << "User sent wrong pass" << std::endl;
		responder(ERR_PASSWDMISMATCH, connected_fd);
		bzero(buff, sizeof(buff));
		// abort_connection(connected_fd);
		close(connected_fd);
		readfds.erase(connected_fd);
		return (1);
	}
	return 0;
}

int IRC::nickCmd(std::vector<std::string> parsed)
{

	std::cout << "USER SENT NICK" << std::endl;
	if (parsed.size() < 2)
	{
		responder(ERR_NONICKNAMEGIVEN, *current_user);
		return 1;
	}
	if (parsed[1].size() > 9)
	{
		responder(ERR_ERRONEUSNICKNAME, *current_user);
		return 1;
	}
	for (std::map<size_t, User>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++)
	{
		if (!parsed[1].compare(it.operator*().second.get_nick()))
		{
			responder(ERR_NICKNAMEINUSE, *current_user);
			return 1;
		}
	}
	current_user->set_nick(parsed[1]);
	return 0;
}

int IRC::userCmd(std::vector<std::string> parsed)
{
	std::cout << "USER SENT USER" << std::endl;
	if (parsed.size() < 2)
	{
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

	if (parsed.size() == 2)
	{
		message.append(parsed[1] + " ");
		message.append(parsed[0] + "\n");
	}
	else
	{
		message.append(raw);
	}
	responder(message, *current_user);
	return 0;
}

int IRC::joinCmd(std::string raw)
{
	std::vector<std::string> params = splitter(raw, ' ');
	std::vector<std::string> channels;
	std::vector<std::string> keys;
	Channel *current_channel;

	if (!params.size())
	{
		responder(ERR_NEEDMOREPARAMS, *current_user);
		return 1;
	}
	channels = splitter(params[0], ',');
	if (params.size() > 1)
		keys = splitter(params[1], ',');
	for (int i = 0; i < channels.size(); i++)
	{
		if (channels[i][0] != '#')
		{
			responder(ERR_NOSUCHCHANNEL, *current_user);
			continue;
		}
		current_channel = &get_channel(channels[i]);
		if (i < keys.size())
		{
			current_channel->userJoin(*current_user, keys[i]);
			namesCmd(*current_channel);
		}
		else
		{
			current_channel->userJoin(*current_user);
			namesCmd(*current_channel);
		}
	}
	return 0;
}

int IRC::quitCmd(std::string raw)
{
	std::vector<std::string> params = splitter(raw, ' ');
	if (!params.size())
		responder(current_user->get_nick(), *current_user);
	else if (current_user)
		responder(params[0], *current_user);
	if (params.size() < 2)
		params.push_back("");
	for (std::map<std::string, Channel *>::iterator it = CHANNEL_MAP.begin(); it != CHANNEL_MAP.end(); it++)
	{
		it.operator*().second->userLeft(*current_user);
	}
	readfds.erase(readfds.find(current_user->get_id()));
	close(current_user->get_id());
	for (std::map<size_t, User>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++)
	{
		if ((*it).second.get_id() != current_user->get_id())
		{
			std::string tmp = ":" + current_user->get_identifier() + " QUIT :" + params[1] + "\n";
			responder(tmp, (*it).second.get_id());
		}
	}
	USER_MAP.erase(USER_MAP.find(current_user->get_id()));
	return 0;
}

int IRC::privmsgCmd(std::string raw)
{
	std::vector<std::string> splitted;
	std::vector<std::string> receivers;
	std::string priv_message;

	splitted = splitter(raw, ' ');
	receivers = splitter(splitted[0], ',');
	// splitted.erase(splitted.begin());
	priv_message = raw.substr(splitted[0].size() + 2, raw.size());
	// for(std::vector<std::string>::iterator it = splitted.begin(); it != splitted.end(); it++)
	// {
	// 	priv_message.append(*it.base());
	// 	if(it != (splitted.end() - 1))
	// 		priv_message.append(" ");
	// }

	std::map<std::string, Channel *>::iterator res;
	bool found = false;

	for (std::vector<std::string>::iterator it = receivers.begin(); it != receivers.end(); it++)
	{
		if ((*it)[0] == '#')
		{
			res = CHANNEL_MAP.find((*it));
			if (res != CHANNEL_MAP.end())
			{
				std::string tmp = ":" + current_user->get_identifier() + " PRIVMSG " + *it + " :" + priv_message + "\n";
				res.operator*().second->globalUserResponder(tmp, current_user->get_id());
			}
			else
			{
				responder(ERR_NOSUCHCHANNEL, *current_user);
			}
		}
		else
		{
			found = false;
			for (std::map<size_t, User>::iterator ite = USER_MAP.begin(); ite != USER_MAP.end(); ite++)
			{
				if (!(*ite).second.get_nick().compare((*it)))
				{
					std::string tmp = ":" + current_user->get_identifier() + " PRIVMSG " + *it + " :" + priv_message + "\n";
					responder(tmp, (*ite).second);
					found = true;
					break;
				}
			}
			if (!found)
				responder(ERR_NOSUCHNICK, *current_user);
		}
	}
	return 0;
}

int IRC::partCmd(std::string raw)
{
	std::vector<std::string> splitted;
	std::vector<std::string> receivers;
	std::string priv_message;

	splitted = splitter(raw, ' ');
	receivers = splitter(splitted[0], ',');
	priv_message = raw.substr(splitted[0].size() + 1, raw.size());

	std::map<std::string, Channel *>::iterator res;
	bool found = false;

	for (std::vector<std::string>::iterator it = receivers.begin(); it != receivers.end(); it++)
	{
		if ((*it)[0] == '#')
		{
			res = CHANNEL_MAP.find((*it));
			if (res != CHANNEL_MAP.end())
			{
				std::string tmp = ":" + current_user->get_identifier() + " PART " + *it + " " + priv_message + "\n";
				// responder(tmp, *current_user);
				(*res).second->globalUserResponder(tmp);
				res.operator*().second->userLeft(*current_user);
			}
			else
			{
				responder(ERR_NOSUCHCHANNEL, *current_user);
			}
		}
	}
	return 0;
}

int IRC::listCmd(std::string raw)
{
	/*start list message*/
	std::string msg = RPL_LISTSTART;
	msg.append(" " + current_user->get_nick() + " Channel :Users Name");
	responder(msg, *current_user);
	/*send actual list*/
	for (std::map<std::string, Channel *>::iterator it = CHANNEL_MAP.begin(); it != CHANNEL_MAP.end(); it++)
	{
		if (!(*it).second->getModes().secret)
		{
			msg.clear();
			msg = RPL_LIST;
			msg.append(" " + current_user->get_nick() + " " + (*it).first + " " + (*it).second->get_user_nb() + " :" + (*it).second->get_modes_str() + " " + (*it).second->get_topic());
			responder(msg, *current_user);
		}
	}
	/*end list message*/
	msg.clear();
	msg = RPL_LISTEND;
	msg.append(" " + current_user->get_nick() + " :End of /LIST");
	responder(msg, *current_user);
	return 0;
}

int IRC::topicCmd(std::string raw)
{
	std::vector<std::string> args = splitter(raw, ' ');
	std::string channel_name = args[0];
	if (args.size() > 1) // setting topic
	{
		std::string topic = raw.substr(channel_name.size() + 1, raw.size());
		for (std::map<std::string, Channel *>::iterator it = CHANNEL_MAP.begin(); it != CHANNEL_MAP.end(); it++)
		{
			if (!channel_name.compare((*it).first))
			{
				(*it).second->set_topic(topic);
				// respond topic is set;
				break;
			}
		}
	}
	else if (args.size() == 1) // show topic
	{
		for (std::map<std::string, Channel *>::iterator it = CHANNEL_MAP.begin(); it != CHANNEL_MAP.end(); it++) // search the channel
		{
			if (!channel_name.compare((*it).first)) // find the channel
			{
				std::vector<size_t> tmp = (*it).second->get_users_ids();
				for (std::vector<size_t>::iterator ite = tmp.begin(); ite != tmp.end(); ite++) // search the user in the channel
				{
					std::string msg = current_user->get_identifier() + " ";
					if (*ite.base() == current_user->get_id() && (*it).second->get_topic().size()) // user in the channel found and topic is set
					{
						msg.append(RPL_TOPIC);
						msg.append(" " + channel_name + " :" + (*it).second->get_topic());
						responder(msg, *current_user);
						return 0;
					}
					else if (*ite.base() != current_user->get_id() && (*it).second->get_topic().size()) // user found but no topic is set
					{
						msg.clear();
						msg.append(RPL_NOTOPIC);
						responder(msg, *current_user);
						return 1;
					}
					else // user in the channel not found
					{
						msg.clear();
						msg.append(ERR_NOTONCHANNEL);
						responder(msg, *current_user);
						return 1;
					}
				}
				return 1;
			}
		}
	}
	else
	{
		std::string msg(std::string(ERR_NEEDMOREPARAMS));
		responder(msg, *current_user);
		return 1;
	}
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
	if (params.size() > 1)
	{
		if (params[0].compare(params[0].size(), 1, "#")) // channel mode
		{
			std::map<std::string, Channel *>::iterator found;
			if ((found = CHANNEL_MAP.find(params[0].substr(1, params[0].size()))) != CHANNEL_MAP.end()) // canale trovato
			{
				if (params[1][0] == '+')
				{
					for (int i = 1; i < params[1].size(); i++)
					{
						if (params[1][i] == 'o')
						{
							if (params.size() > 2)
								(*found).second->setOp(true, params[2]);
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else if (params[1][i] == 'p')
							(*found).second->setPrivate();
						else if (params[1][i] == 's')
							(*found).second->setSecret();
						else if (params[1][i] == 'i')
							(*found).second->setInviteOnly();
						else if (params[1][i] == 't')
							(*found).second->setNoOpTopic();
						else if (params[1][i] == 'n')
							(*found).second->setNoExternalMessages();
						else if (params[1][i] == 'm')
							(*found).second->setModerated();
						else if (params[1][i] == 'l')
						{
							if (params.size() > 2)
								(*found).second->setUserLimit(true, atoi(params[2].c_str()));
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else if (params[1][i] == 'b')
						{
							if (params.size() > 2)
								(*found).second->setBanMask(params[2]);
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else if (params[1][i] == 'v')
						{
							if (params.size() > 2)
								(*found).second->setSpeak(true, params[2]);
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else if (params[1][i] == 'k')
							(*found).second->setKey();
					}
				}
				else if (params[1][0] == '-')
				{
					for (int i = 1; i < params[1].size(); i++)
					{
						if (params[1][i] == 'o')
							(*found).second->setNoOpTopic(false);
						else if (params[1][i] == 'p')
							(*found).second->setPrivate(false);
						else if (params[1][i] == 's')
							(*found).second->setSecret(false);
						else if (params[1][i] == 'i')
							(*found).second->setInviteOnly(false);
						else if (params[1][i] == 't')
							(*found).second->setTopic(params[2]);
						else if (params[1][i] == 'n')
							(*found).second->setNoExternalMessages(false);
						else if (params[1][i] == 'm')
							(*found).second->setModerated(false);
						else if (params[1][i] == 'l')
							(*found).second->setUserLimit(false);
						else if (params[1][i] == 'b')
							(*found).second->setBanMask(params[2]);
						else if (params[1][i] == 'v' && (*found).second->getModes().moderate == true)
							(*found).second->setSpeak(false);
						else if (params[1][i] == 'k')
							(*found).second->setKey(false);
					}
				}
			}
		}
		else // user mode
		{
		}
	}
	else
	{
		std::string resp = "0.0.0.falso " + std::string(RPL_UMODEIS) + " " + current_user->get_nick() + " +";
		// responder();
	}
	// error ERR_NEEDMOREPARAMS
	return 1;
}

/*4.5.1 Who query

	  Command: WHO
   Parameters: [<name> [<o>]]

   The WHO message is used by a client to generate a query which returns
   a list of information which 'matches' the <name> parameter given by
   the client.  In the absence of the <name> parameter, all visible
   (users who aren't invisible (user mode +i) and who don't have a
   common channel with the requesting client) are listed.  The same
   result can be achieved by using a <name> of "0" or any wildcard which



Oikarinen & Reed                                               [Page 33]

RFC 1459              Internet Relay Chat Protocol              May 1993


   will end up matching every entry possible.

   The <name> passed to WHO is matched against users' host, server, real
   name and nickname if the channel <name> cannot be found.

   If the "o" parameter is passed only operators are returned according
   to the name mask supplied.

   Numeric Replies:

		   ERR_NOSUCHSERVER
		   RPL_WHOREPLY                    RPL_ENDOFWHO

   Examples:

   WHO *.fi                        ; List all users who match against
								   "*.fi".

   WHO jto* o                      ; List all users with a match against
								   "jto*" if they are an operator.
*/

int IRC::namesCmd(Channel curr_channel)
{
	std::string msg;
	std::vector<size_t> whoInTheChann = curr_channel.get_users_ids();
	for (int i = 0; i < whoInTheChann.size(); i++) /*send Namelist*/
	{
		msg.append(":" + current_user->get_remote_ip() + " ");
		msg.append(RPL_NAMREPLY);
		std::map<size_t, User>::iterator found = USER_MAP.find(whoInTheChann[i]);
		msg.append(" " + current_user->get_nick() + " = " + curr_channel.get_name() + " :" + (*found).second.get_nick());
		responder(msg, *current_user);
		msg.clear();
	}
	/*send endNamelist*/
	msg.clear();
	// msg.append(":0.0.0.falso");
	msg.append(":" + current_user->get_remote_ip() + " ");
	msg.append(RPL_ENDOFNAME);
	msg.append(" " + current_user->get_nick() + " " + curr_channel.get_name() + " :End of NAMES list");
	responder(msg, *current_user);
	return 0;
}

int IRC::whoCmd(std::string raw)
{
	if (raw.size())
	{
		if (raw[0] == '#')
		{
			for (std::map<std::string, Channel *>::iterator it = CHANNEL_MAP.begin(); it != CHANNEL_MAP.end(); it++)
			{
				if (!(*it).first.compare(raw))
				{
					std::string msg;
					std::vector<size_t> whoInTheChann = (*it).second->get_users_ids();
					for (int i = 0; i < whoInTheChann.size(); i++) /*send channel wholist*/
					{
						// msg.append(":0.0.0.falso ");
						msg.append(":" + current_user->get_remote_ip() + " ");
						msg.append(RPL_WHOREPLY);
						std::map<size_t, User>::iterator found = USER_MAP.find(whoInTheChann[i]);
						if (!(*found).second._state.invisible && current_user->get_id() != (*found).first)
						{
							msg.append(" " + current_user->get_nick() + " " + (*it).first + " " + (*found).second.get_username() + " " + (*found).second.get_remote_ip() + " " + (*found).second.get_nick() + " H :0" + (*found).second.get_realname());
							responder(msg, *current_user);
						}
						msg.clear();
					}
					/*send endlist*/
					msg.clear();
					// msg.append(":0.0.0.falso ");
					msg.append(":" + current_user->get_remote_ip() + " ");
					msg.append(RPL_ENDOFWHO);
					msg.append(" " + current_user->get_nick() + " " + (*it).first + " :End of WHO list");
					responder(msg, *current_user);
					break;
				}
			}
		}
	}
	else
	{
		responder(ERR_NEEDMOREPARAMS, *current_user);
	}
	return 0;
}

#endif /*COMMAND_HPP*/
