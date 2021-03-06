#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include "irc.hpp"
#include <iostream>
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
		return 2;
	}
	return 0;
}

std::string &capitalize(std::string &str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		str[i] = toupper(str[i]);
	}
	return str;
}

void IRC::commandSelector(std::string raw)
{
	std::vector<std::string> parsed;
	std::string command;
	std::string params = "";

	current_user = get_user(connected_fd);
	parsed = splitter(raw, ' ');
	command = capitalize(parsed[0]);
	if (parsed.size() > 1 && parsed[1].size())
		params = raw.substr(command.size() + 1, raw.size());
	if (!command.compare("PING"))
		pongCmd(params);
	else if (!command.compare("JOIN"))
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
	else if (!parsed[0].compare("MODE"))
		modeCmd(params);
	else if (!parsed[0].compare("MOTD"))
		motdCmd(params);
	else if (!parsed[0].compare("NOTICE"))
		noticeCmd(params);
	else if (!parsed[0].compare("KICK"))
		kickCmd(params);
	else if (!parsed[0].compare("INVITE"))
		inviteCmd(params);
}

void IRC::inviteCmd(std::string raw)
{
	std::vector<std::string> parsed = splitter(raw, ' ');
	if (parsed.size() < 2)
	{
		responder(ERR_NEEDMOREPARAMS, connected_fd);
		return;
	}
	User *dest = get_user(parsed[0]);
	if (!dest)
	{
		responder(ERR_NOSUCHNICK, connected_fd);
		return;
	}
	std::map<std::string, Channel *>::iterator chan_it = CHANNEL_MAP.find(parsed[1]);
	if (chan_it == CHANNEL_MAP.end())
		return;
	if (!chan_it->second->is_in_channel(current_user->get_id()))
	{
		responder(ERR_NOTONCHANNEL, connected_fd);
		return;
	}
	if (chan_it->second->is_in_channel(dest->get_id()))
	{
		responder(ERR_USERONCHANNEL, connected_fd);
		return;
	}
	if (chan_it->second->getModes().invite && !chan_it->second->userIsOp(*current_user))
	{
		responder(ERR_CHANOPRIVSNEEDED, connected_fd);
		return;
	}
	std::string rep = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + RPL_INVITING + " " + current_user->get_nick() + " " + dest->get_nick() + " " + chan_it->second->get_name();
	responder(rep, *current_user);
	rep = ":" + current_user->get_identifier() + " INVITE " + dest->get_nick() + " :" + chan_it->second->get_name();
	responder(rep, *dest);
	chan_it->second->setUserInvited(dest);
}

int IRC::passCmd(std::vector<std::string> parsed)
{
	if ((!parsed[0].compare("PASS") && parsed.size() < 2) || (parsed[0].compare("PASS")))
	{
		std::cout << "User sent no pass" << std::endl;
		responder(ERR_NEEDMOREPARAMS, connected_fd);
		close(connected_fd);
		readfds.erase(connected_fd);
		return (1);
	}
	if (parsed[1].compare(":" + this->_password) && parsed[1].compare(this->_password))
	{
		std::cout << "User sent wrong pass" << std::endl;
		responder(ERR_PASSWDMISMATCH, connected_fd);
		bzero(buff, sizeof(buff));
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
	for (std::map<std::string, Channel *>::iterator it = CHANNEL_MAP.begin(); it != CHANNEL_MAP.end(); it++) {
		if (it->second->is_in_channel(current_user->get_id())) {
			std::string msg = ":" + current_user->get_identifier() + " NICK :" + parsed[1];
			it->second->globalUserResponder(msg);
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
		message.append(parsed[0]);
	}
	else
	{
		message.append(raw);
	}
	responder(message, *current_user);
	return 0;
}

int IRC::motdCmd(std::string raw)
{
	std::ifstream motd;
	(void)raw;
	motd.open("MOTD.txt", std::ios::in);
	if (!motd)
	{
		std::string msg = std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_NOMOTD) + " " + current_user->get_nick() + " :No MOTD file";
		responder(msg, *current_user);
		return 1;
	}
	std::string msg = std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(RPL_MOTDSTART) + " " + current_user->get_nick() + " :MOTD message start";
	responder(msg, *current_user);
	msg.clear();
	msg = std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(RPL_MOTD) + " " + current_user->get_nick() + " :";
	std::string tmp;
	while (std::getline(motd, tmp))
	{
		responder(msg + tmp, *current_user);
	}
	msg.clear();
	msg = std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(RPL_ENDOFMOTD) + " " + current_user->get_nick() + " :End of /MOTD command";
	responder(msg, *current_user);
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
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i][0] != '#')
		{
			responder(ERR_NOSUCHCHANNEL, *current_user);
			continue;
		}
		current_channel = &get_channel(channels[i]);
		if (i < keys.size())
		{
			if (current_channel->getModes().has_limit && (int)current_channel->get_limit() == atoi(current_channel->get_user_nb().c_str())) //userlimit is set and full capacity reached
			{
				std::string msg = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_CHANNELISFULL) + current_channel->get_name() + " :Cannot join the channel " + current_channel->get_modes_str("(", ")");
				responder(msg, *current_user);
				continue;
			}
			if (current_channel->getModes().invite && !current_channel->is_invited(current_user->get_id())) //only invide mode is checked and user is searched in invite list
			{
				std::string msg = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_INVITEONLYCHAN) + current_channel->get_name() + " :Cannot join the channel " + current_channel->get_modes_str("(", ")");
				responder(msg, *current_user);
				continue;
			}
			if (!current_channel->userJoin(*current_user, keys[i]))
				namesCmd(*current_channel);
		}
		else
		{
			if (current_channel->getModes().has_limit && (int)current_channel->get_limit() == atoi(current_channel->get_user_nb().c_str())) //userlimit is set and full capacity reached
			{
				std::string msg = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_CHANNELISFULL) + current_channel->get_name() + " :Cannot join the channel " + current_channel->get_modes_str("(", ")");
				responder(msg, *current_user);
				continue;
			}
			if (current_channel->getModes().invite && !current_channel->is_invited(current_user->get_id()))
			{
				std::string msg = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_INVITEONLYCHAN) + " " + current_channel->get_name() + " :Cannot join the channel " + current_channel->get_modes_str("(", ")");
				responder(msg, *current_user);
				continue;
			}
			if (!current_channel->userJoin(*current_user))
				namesCmd(*current_channel);
		}
		topicCmd(current_channel->get_name());
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
	std::map<std::string, Channel *>::iterator it = CHANNEL_MAP.begin();
	std::map<std::string, Channel *>::iterator tmp;
	while (it != CHANNEL_MAP.end())
	{
		it.operator*().second->userLeft(*current_user);
		if (atoi((*it).second->get_user_nb().c_str()) == 0)
		{
			tmp = it;
			tmp++;
			CHANNEL_MAP.erase(it);
			delete ((*it).second);
			it = tmp;
		}
		else
			it++;
	}

	readfds.erase(readfds.find(current_user->get_id()));
	close(current_user->get_id());
	USER_MAP.erase(USER_MAP.find(current_user->get_id()));
	for (std::map<size_t, User>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++)
	{
		if ((*it).second.get_id() != current_user->get_id())
		{
			std::string tmp = ":" + current_user->get_identifier() + " QUIT :" + params[1] + "\n";
			responder(tmp, (*it).second.get_id());
		}
	}
	return 0;
}

int IRC::privmsgCmd(std::string raw)
{
	std::vector<std::string> splitted;
	std::vector<std::string> receivers;
	std::string priv_message;

	splitted = splitter(raw, ' ');
	receivers = splitter(splitted[0], ',');
	if (splitted.size() > 1)
		priv_message = raw.substr(splitted[0].size() + 2, raw.size());

	std::map<std::string, Channel *>::iterator res;
	bool found = false;

	for (std::vector<std::string>::iterator it = receivers.begin(); it != receivers.end(); it++)
	{
		if ((*it)[0] == '#')
		{
			res = CHANNEL_MAP.find((*it));
			if (res != CHANNEL_MAP.end())
			{
				if (!(*res).second->getModes().no_ext || ((*res).second->getModes().no_ext && (*res).second->is_in_channel(current_user->get_id())))
				{
					if ((!(*res).second->getModes().moderate) || (((*res).second->getModes().moderate) && ((*res).second->userHasVoice(*current_user) || (*res).second->userIsOp(*current_user))))
					{
						std::string tmp = ":" + current_user->get_identifier() + " PRIVMSG " + *it + " :" + priv_message;
						res.operator*().second->globalUserResponder(tmp, current_user->get_id());
					}
					else
					{
						std::string tmp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_CANNOTSENDTOCHAN) + " " + current_user->get_nick() + " " + (*res).first + " :No external channel message (" + (*res).first + ")";
						responder(tmp, *current_user);
					}
				}
				else
				{
					std::string tmp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_CANNOTSENDTOCHAN) + " " + current_user->get_nick() + " " + (*res).first + " :No external channel message (" + (*res).first + ")";
					responder(tmp, *current_user);
					break;
				}
			}
			else
			{
				std::string tmp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_NOSUCHCHANNEL) + " " + current_user->get_nick() + " " + (*it) + " :No such channel";
				responder(tmp, *current_user);
			}
		}
		else
		{
			found = false;
			for (std::map<size_t, User>::iterator ite = USER_MAP.begin(); ite != USER_MAP.end(); ite++)
			{
				if (!(*ite).second.get_nick().compare((*it)))
				{
					std::string tmp = ":" + current_user->get_identifier() + " PRIVMSG " + *it + " :" + priv_message;
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

int IRC::noticeCmd(std::string raw)
{
	std::vector<std::string> splitted;
	std::vector<std::string> receivers;
	std::string priv_message;

	splitted = splitter(raw, ' ');
	receivers = splitter(splitted[0], ',');
	if (splitted.size() > 1)
		priv_message = raw.substr(splitted[0].size() + 1, raw.size());

	std::map<std::string, Channel *>::iterator res;
	bool found = false;

	for (std::vector<std::string>::iterator it = receivers.begin(); it != receivers.end(); it++)
	{
		found = false;
		for (std::map<size_t, User>::iterator ite = USER_MAP.begin(); ite != USER_MAP.end(); ite++)
		{
			if (!(*ite).second.get_nick().compare((*it)))
			{
				std::string tmp = ":" + current_user->get_identifier() + " NOTICE " + *it + " :" + priv_message;
				responder(tmp, (*ite).second);
				found = true;
				break;
			}
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

	for (std::vector<std::string>::iterator it = receivers.begin(); it != receivers.end(); it++)
	{
		if ((*it)[0] == '#')
		{
			res = CHANNEL_MAP.find((*it));
			if (res != CHANNEL_MAP.end())
			{
				std::string tmp = ":" + current_user->get_identifier() + " PART " + *it + " " + priv_message;
				(*res).second->globalUserResponder(tmp);
				res.operator*().second->userLeft(*current_user);
				if (atoi((*res).second->get_user_nb().c_str()) == 0) //elimina canale se vuoto
					CHANNEL_MAP.erase((*res).first);
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
	std::string msg;
	(void)raw;
	msg.append(":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(RPL_LISTSTART) + " " + current_user->get_nick() + " Channel :Users  Name");
	responder(msg, *current_user);
	/*send actual list*/
	for (std::map<std::string, Channel *>::iterator it = CHANNEL_MAP.begin(); it != CHANNEL_MAP.end(); it++)
	{
		if (!(*it).second->getModes().secret)
		{
			msg.clear();
			msg.append(":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(RPL_LIST));
			msg.append(" " + current_user->get_nick() + " " + (*it).first + " " + (*it).second->get_user_nb() + " :");
			if ((*it).second->getModes().priv && !(*it).second->is_in_channel(current_user->get_id()))
			{
				responder(msg, *current_user);
			}
			else
				responder(msg + (*it).second->get_topic(), *current_user);
		}
	}
	/*end list message*/
	msg.clear();
	msg.append(":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(RPL_LISTEND));
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
				if ((!(*it).second->getModes().no_op_topic) || ((*it).second->getModes().no_op_topic && (*it).second->userIsOp(*current_user)))
				{
					(*it).second->setTopic(topic.substr(1, topic.size()), current_user);
					std::string msg = ":" + current_user->get_identifier() + " TOPIC " + (*it).first + " :" + (*it).second->get_topic();
					(*it).second->globalUserResponder(msg);
					break;
				}
				else
				{
					std::string msg = ":" + std::string(inet_ntoa(remote.sin_addr)) + std::string(ERR_CHANOPRIVSNEEDED) + " " + current_user->get_nick() + " " + channel_name + " :You're not channel operator";
					(*it).second->globalUserResponder(msg);
					break;
				}
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
				std::string msg = ":" + std::string(inet_ntoa(remote.sin_addr)) + " ";
				for (std::vector<size_t>::iterator ite = tmp.begin(); ite != tmp.end(); ite++) // search the user in the channel
				{
					if (*ite == current_user->get_id() && (*it).second->get_topic().size()) // user in the channel found and topic is set
					{
						msg.append(RPL_TOPIC);
						msg.append(" " + current_user->get_nick() + " " + (*it).second->get_name() + " :" + (*it).second->get_topic());
						responder(msg, *current_user);
						return 0;
					}
					else if (*ite == current_user->get_id() && !(*it).second->get_topic().size()) // user found but no topic is set
					{
						msg.clear();
						msg.append(RPL_NOTOPIC);
						responder(msg, *current_user);
						return 1;
					}
					continue;
				}
				msg.clear();
				msg.append(ERR_NOTONCHANNEL);
				responder(msg, *current_user);
				return 1;
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
	std::string old_modes;
	std::string new_modes;
	params = splitter(raw, ' ');
	if (params.size() > 1)
	{
		if (params[0][0] == '#') // channel mode
		{
			std::map<std::string, Channel *>::iterator found;
			if ((found = CHANNEL_MAP.find(params[0])) != CHANNEL_MAP.end()) // canale trovato
			{
				if (params[1][0] == '+')
				{
					old_modes = found->second->get_modes_str("", "");
					for (size_t i = 1; i < params[1].size(); i++)
					{
						if (params[1][i] == 'o')
						{
							if (params.size() > 2)
								(*found).second->setOp(true, current_user, get_user(params[2]));
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else if (params[1][i] == 'p')
							(*found).second->setPrivate(true, current_user);
						else if (params[1][i] == 's')
							(*found).second->setSecret(true, current_user);
						else if (params[1][i] == 'i')
							(*found).second->setInviteOnly(true, current_user);
						else if (params[1][i] == 't')
							(*found).second->setNoOpTopic(true, current_user);
						else if (params[1][i] == 'n')
							(*found).second->setNoExternalMessages(true, current_user);
						else if (params[1][i] == 'm')
							(*found).second->setModerated(true, current_user);
						else if (params[1][i] == 'l')
						{
							if (params.size() > 2)
								(*found).second->setUserLimit(true, current_user, atoi(params[2].c_str()));
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else if (params[1][i] == 'b')
						{
							if (params.size() > 2)
							{
								(*found).second->setBanMask(true, params[2], current_user);
							}
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else if (params[1][i] == 'v')
						{
							if (params.size() > 2)
								(*found).second->setVoice(true, current_user, get_user(params[2]));
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else if (params[1][i] == 'k')
						{
							if (params.size() > 2)
								(*found).second->setKey(true, current_user, params[2]);
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else
						{
							std::string tmp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_UNKNOWNMODE) + " " + current_user->get_nick() + " " + params[1][i] + " :Unknown mode char";
							responder(tmp, *current_user);
						}
					}
					new_modes = found->second->get_modes_str("", "");
					if (old_modes != new_modes)
					{
						Channel *ch;
						std::string if_limit = found->second->getModes().has_limit ? SSTR(found->second->get_limit()) : "";
						if (found != CHANNEL_MAP.end()) // canale trovato
						{
							ch = found->second;
							std::string resp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(RPL_CHANNELMODEIS) + " " + current_user->get_nick() + " " + found->second->get_name() + " ";
							ch->globalUserResponder(resp + ch->get_modes_str("", "") + " " + if_limit);
							return 0;
						}
					}
				}
				else if (params[1][0] == '-')
				{
					old_modes = found->second->get_modes_str("", "");
					for (size_t i = 1; i < params[1].size(); i++)
					{
						if (params[1][i] == 'o')
						{
							if (params.size() > 2)
								(*found).second->setOp(false, current_user, get_user(params[2]));
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else if (params[1][i] == 'p')
							(*found).second->setPrivate(false, current_user);
						else if (params[1][i] == 's')
							(*found).second->setSecret(false, current_user);
						else if (params[1][i] == 'i')
							(*found).second->setInviteOnly(false, current_user);
						else if (params[1][i] == 't')
							(*found).second->setNoOpTopic(false, current_user);
						else if (params[1][i] == 'n')
							(*found).second->setNoExternalMessages(false, current_user);
						else if (params[1][i] == 'm')
							(*found).second->setModerated(false, current_user);
						else if (params[1][i] == 'l')
							(*found).second->setUserLimit(false, current_user, 0);
						else if (params[1][i] == 'b')
						{
							if (params.size() > 2)
								(*found).second->setBanMask(false, params[2], current_user);
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else if (params[1][i] == 'v')
						{
							if (params.size() > 2)
								(*found).second->setVoice(false, current_user, get_user(params[2]));
							else
								responder(ERR_NEEDMOREPARAMS, *current_user);
						}
						else if (params[1][i] == 'k')
							(*found).second->setKey(false, current_user);
						else
						{
							std::string tmp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_UNKNOWNMODE) + " " + current_user->get_nick() + " " + params[1][i] + " :Unknown mode char";
							responder(tmp, *current_user);
						}
					}
					new_modes = found->second->get_modes_str("", "");
					std::string diff_modes;
					for (size_t i = 0; i < old_modes.size(); i++)
					{
						if (new_modes.find(old_modes[i]) == std::string::npos)
							diff_modes.push_back(old_modes[i]);
					}
					if (old_modes != new_modes)
					{
						Channel *ch;
						if (found != CHANNEL_MAP.end()) // canale trovato
						{
							ch = found->second;
							std::string resp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(RPL_CHANNELMODEIS) + " " + current_user->get_nick() + " " + found->second->get_name() + " ";
							ch->globalUserResponder(resp + "-" + diff_modes);
							return 0;
						}
					}
				}
				else if (params[1][0] == 'b')
				{
					std::string msg;
					std::string is_op;
					std::vector<std::string> bans = found->second->get_bans();
					for (size_t i = 0; i < bans.size(); i++) /*send ban list*/
					{
						msg.append(":" + std::string(inet_ntoa(remote.sin_addr)) + " ");
						msg.append(RPL_BANLIST);
						msg.append(" " + current_user->get_nick() + " " + found->second->get_name() + " " + bans[i]);
						responder(msg, *current_user);
						msg.clear();
					}
					/*send endNamelist*/
					msg.clear();
					msg.append(":" + std::string(inet_ntoa(remote.sin_addr)) + " ");
					msg.append(RPL_ENDOFBANLIST);
					msg.append(" " + current_user->get_nick() + " " + found->second->get_name() + " :End of Channel Ban List");
					responder(msg, *current_user);
				}
			}
		}
		else // user mode
		{
			if (params[0].compare(current_user->get_nick())) //check if the sender and the name given as parameter are the same
			{
				std::string tmp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_USERSDONTMATCH) + " " + current_user->get_nick() + " :Users does not matches";
				responder(tmp, *current_user);
				return 1;
			}
			if (params[1][0] == '+')
			{
				for (size_t i = 1; i < params[1].size(); i++)
				{
					if (params[1][i] == 'i')
						(*current_user)._state.invisible = true;
					//autonomous opping is ignored
					else
					{
						std::string tmp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_UMODEUNKNOWNFLAG) + " " + current_user->get_nick() + " " + params[1][i] + " :Unknown mode char";
						responder(tmp, *current_user);
					}
				}
			}
			else if (params[1][0] == '-')
			{
				for (size_t i = 1; i < params[1].size(); i++)
				{
					if (params[1][i] == 'i')
						(*current_user)._state.invisible = false;
					else if (params[1][i] == 'o')
						(*current_user)._state.op = false;
					else
					{
						std::string tmp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_UMODEUNKNOWNFLAG) + " " + current_user->get_nick() + " " + params[1][i] + " :Unknown mode char";
						responder(tmp, *current_user);
					}
				}
				return 1;
			}
		}
	}
	else if (params[0].size() > 0)
	{
		if (params[0][0] == '#')
		{
			std::map<std::string, Channel *>::iterator found = CHANNEL_MAP.find(params[0]);
			Channel *ch;
			if (found != CHANNEL_MAP.end()) // canale trovato
			{
				ch = found->second;
				std::string resp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(RPL_CHANNELMODEIS) + " " + current_user->get_nick() + " " + found->second->get_name() + " ";
				responder(resp + ch->get_modes_str("", ""), *current_user);
				return 0;
			}
		}
		else
		{
			if (!current_user->get_nick().compare(params[0]))
			{
				std::string resp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(RPL_UMODEIS) + " " + current_user->get_nick() + " ";
				responder(resp + current_user->get_user_modes(), *current_user);
				return 0;
			}
		}
	}
	else
	{
		std::string resp = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_NEEDMOREPARAMS) + " " + current_user->get_nick() + " ";
		responder(resp, *current_user);
	}
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
	std::string is_op;
	std::vector<size_t> whoInTheChann = curr_channel.get_users_ids();
	for (size_t i = 0; i < whoInTheChann.size(); i++) /*send Namelist*/
	{
		msg.append(":" + current_user->get_remote_ip() + " ");
		msg.append(RPL_NAMREPLY);
		std::map<size_t, User>::iterator found = USER_MAP.find(whoInTheChann[i]);
		is_op = curr_channel.userIsOp(found->second) ? "@" : curr_channel.userHasVoice(found->second) ? "+"
																									  : "";
		msg.append(" " + current_user->get_nick() + " = " + curr_channel.get_name() + " :" + is_op + (*found).second.get_nick());
		responder(msg, *current_user);
		msg.clear();
	}
	/*send endNamelist*/
	msg.clear();
	msg.append(":" + current_user->get_remote_ip() + " ");
	msg.append(RPL_ENDOFNAME);
	is_op = curr_channel.userIsOp(*current_user) ? "@" : curr_channel.userHasVoice(*current_user) ? "+"
																								  : "";
	msg.append(" " + current_user->get_nick() + " " + curr_channel.get_name() + " :End of NAMES list");
	responder(msg, *current_user);
	return 0;
}

int IRC::whoCmd(std::string raw)
{
	std::string is_op;
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
					for (size_t i = 0; i < whoInTheChann.size(); i++) /*send channel wholist*/
					{
						msg.append(":" + current_user->get_remote_ip() + " ");
						msg.append(RPL_WHOREPLY);
						std::map<size_t, User>::iterator found = USER_MAP.find(whoInTheChann[i]);
						if (!(*found).second._state.invisible && current_user->get_id() != (*found).first)
						{
							is_op = it->second->userIsOp(found->second) ? "@" : it->second->userHasVoice(found->second) ? "+"
																														: "";
							msg.append(" " + current_user->get_nick() + " " + (*it).first + " " + (*found).second.get_username() + " " + (*found).second.get_remote_ip() + " " + (*found).second.get_nick() + " H" + is_op + " :0" + (*found).second.get_realname());
							responder(msg, *current_user);
						}
						msg.clear();
					}
					/*send endlist*/
					msg.clear();
					is_op = it->second->userIsOp(*current_user) ? "@" : it->second->userHasVoice(*current_user) ? "+"
																												: "";
					msg.append(":" + current_user->get_remote_ip() + " ");
					msg.append(RPL_ENDOFWHO);
					msg.append(" " + is_op + current_user->get_nick() + " " + (*it).first + " :End of WHO list");
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

int IRC::kickCmd(std::string raw)
{
	std::vector<std::string> splitted;
	std::string toKick;
	std::string channel;
	std::string comment;

	splitted = splitter(raw, ' ');
	channel = splitted[0];
	if (splitted.size() > 1)
		toKick = splitted[1];
	if (splitted.size() > 2)
		comment = raw.substr(splitted[0].size() + splitted[1].size() + 2, raw.size());

	if (splitted.size() < 2)
	{
		std::string msg = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_NEEDMOREPARAMS) + current_user->get_nick() + " :Need more params";
		responder(msg, *current_user);
		return 1;
	}
	else
	{
		std::map<std::string, Channel *>::iterator foundchan = CHANNEL_MAP.find(channel);
		if (foundchan != CHANNEL_MAP.end())
		{
			if ((*foundchan).second->userIsOp(*current_user))
			{
				User *UserToKick = get_user(toKick);
				if (UserToKick && (*foundchan).second->is_in_channel(UserToKick->get_id()))
				{
					std::string msg = ":" + (*current_user).get_identifier() + " KICK " + (*foundchan).second->get_name() + " " + UserToKick->get_nick() + " " + comment;
					(*foundchan).second->globalUserResponder(msg);
					(*foundchan).second->userLeft(*UserToKick);
				}
				else
				{
					//error ERR_NOTONCHANNEL
					std::string msg = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_NOTONCHANNEL) + " " + current_user->get_nick() + " :Not on channel";
					responder(msg, *current_user);
					return 1;
				}
			}
			else
			{
				//error ERR_CHANOPRIVSNEEDED
				std::string msg = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_CHANOPRIVSNEEDED) + " " + current_user->get_nick() + " :No op right";
				responder(msg, *current_user);
				return 1;
			}
		}
		else
		{
			//error ERR_NOSUCHCHANNEL
			std::string msg = ":" + std::string(inet_ntoa(remote.sin_addr)) + " " + std::string(ERR_NOSUCHCHANNEL) + " " + current_user->get_nick() + " :No such channel";
			responder(msg, *current_user);
			return 1;
		}
	}
	return 0;
}

#endif /*COMMAND_HPP*/
