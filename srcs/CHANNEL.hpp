#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#define SSTR(x) static_cast<std::ostringstream &>((std::ostringstream() << std::dec << x)).str()

#include <iostream>
#include <list>
#include <set>
#include "USER.hpp"
#include "errors.hpp"
#include "utils.hpp"
#include <ctime>
#include <stdio.h>

typedef struct s_channel_modes
{
	bool priv;
	bool secret;
	bool invite;
	bool topic;
	bool no_ext;
	bool moderate;
	bool has_limit;
	bool has_key;
	bool voice;
	bool no_op_topic;
} t_channel_modes;

typedef struct s_user_status
{
	bool admin;
	bool voice;
} t_user_status;

class Channel
{
private:
	// size_t					_id;
	std::string _name;
	std::string _key;
	std::string _topic;
	std::map<size_t, t_user_status> USER_MAP; //mappa con user right come key e utente corrispondende
	std::set<size_t> invited_users;
	size_t user_limit;
	t_channel_modes modes;
	std::vector<std::string> ban_masks;
	time_t creation_time;

public:
	Channel(std::string);
	~Channel();

	void userJoin(User &, std::string);
	void userLeft(User &);
	void userBan(User &);
	void userOp(User &);
	void userKick(User &);

	bool userIsOp(User &);
	bool userHasVoice(User &);

	t_channel_modes getModes() const;
	void setTopic(std::string, User *);
	void globalUserResponder(std::string, size_t);

	void setOp(bool, User *, User *);
	void setPrivate(bool, User *);
	void setSecret(bool, User *);
	void setInviteOnly(bool, User *);
	void setNoOpTopic(bool, User *);
	void setNoExternalMessages(bool, User *);
	void setModerated(bool, User *);
	void setVoice(bool, User *, User *);
	void setUserLimit(bool, User *, size_t);
	void setKey(bool, User *, std::string);
	void setBanMask(std::string, User *);
	std::string get_user_nb();
	std::string get_modes_str(std::string, std::string);
	std::string get_modes_user_str(User &user);
	std::string get_topic();
	std::string get_name();
	std::vector<size_t> get_users_ids();
	std::string get_creation_time();
	bool is_invited(size_t);
	bool is_in_channel(size_t user_id);
};

Channel::Channel(std::string channel_name)
{
	_name = channel_name;
	_key = "";
	_topic = "";
	user_limit = 0;
	modes = (t_channel_modes){false, false, false, true, true, false, false, true};
	creation_time = std::time(NULL);
};

Channel::~Channel(){};

std::string Channel::get_user_nb()
{
	std::stringstream connectedUsersNb;
	connectedUsersNb << USER_MAP.size();
	std::string number = connectedUsersNb.str();
	return (number);
}

std::string Channel::get_name()
{
	return (_name);
}

/*
	o - give/take channel operator privileges;
	-p - private channel flag;
	-s - secret channel flag;
	-i - invite-only channel flag;
	-t - topic settable by channel operator only flag;
	-n - no messages to channel from clients on the outside;
	-m - moderated channel;
	-l - set the user limit to channel;
	b - set a ban mask to keep users out;
	v - give/take the ability to speak on a moderated channel;
	-k - set a channel key (password).
*/

std::string Channel::get_modes_str(std::string prepend_str = "[", std::string append_str = "]")
{
	std::string res = prepend_str + "+";
	if (modes.has_key)
		res.append("k");
	if (modes.has_limit)
		res.append("l");
	if (modes.invite)
		res.append("i");
	if (modes.moderate)
		res.append("m");
	if (modes.no_ext)
		res.append("n");
	if (modes.priv)
		res.append("p");
	if (modes.secret)
		res.append("s");
	if (modes.topic)
		res.append("t");
	res.append(append_str);
	return (res);
}

std::string Channel::get_modes_user_str(User &user)
{
	std::string res = "+";
	if (!USER_MAP.count(user.get_id())) {
		std::cout << "User not found\n" << std::endl;
		return "";
	}
	t_user_status user_modes = USER_MAP[user.get_id()];
	if (user_modes.admin)
		res.append("o");
	if (user_modes.voice)
		res.append("o");
	return (res);
}

bool Channel::is_invited(size_t user_id)
{
	for(std::set<size_t>::iterator it = invited_users.begin(); it != invited_users.end(); it++)
	{
		if((*it) == user_id)
		{
			invited_users.erase(user_id);
			return(true);
		}
	}
	return(false);
}

bool Channel::is_in_channel(size_t user_id)
{
	for(std::map<size_t, t_user_status>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++)
	{
		if((*it).first == user_id)
			return(true);
	}
	return(false);
}

std::string Channel::get_topic() { return (_topic); }

std::vector<size_t> Channel::get_users_ids()
{
	std::vector<size_t> res;
	for (std::map<size_t, t_user_status>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++)
		res.push_back((*it).first);
	return (res);
}

void Channel::userJoin(User &user, std::string pass = "")
{
	t_user_status stat((t_user_status){false, false});
	if (modes.has_key && pass != _key)
	{
		responder(ERR_BADCHANNELKEY, user);
		return;
	}
	if (modes.has_limit && USER_MAP.size() >= user_limit)
	{
		responder(ERR_CHANNELISFULL, user);
		return;
	}
	if (modes.invite)
	{
		if (!invited_users.count(user.get_id()))
		{
			responder(ERR_INVITEONLYCHAN, user);
			return;
		}
		invited_users.erase(user.get_id()); // todo: delete fd when disconnected
	}
	if (USER_MAP.find(user.get_id()) != USER_MAP.end())
	{
		responder(ERR_ALREADYREGISTRED, user);
		return;
	}
	if (!USER_MAP.size())
		stat.admin = true;
	std::pair<std::map<size_t, t_user_status>::iterator, bool> ritorno = USER_MAP.insert(std::pair<size_t, t_user_status>(user.get_id(), stat));
	std::cout << "RITORNO ID: " << ritorno.first.operator*().first << " IS_NEW: " << ritorno.second << std::endl;
	std::cout << "User " << user.get_nick() << " joined channel " << _name << std::endl;
	for (std::map<size_t, t_user_status>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++) //communicate to all the user connected to the same channel that the new user connected
	{
		responder(":" + user.get_identifier() + " JOIN " + this->_name, (*it).first);
		std::cout << "[SENT JOIN MESSAGE TO]: " << (*it).first << " Size: " << USER_MAP.size() << std::endl;
	}
	// todo: RPL_TOPIC
	// todo: RPL_NAMREPLY list users including me
}

void Channel::globalUserResponder(std::string message, size_t skip = 0)
{
	for (std::map<size_t, t_user_status>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++)
	{
		if ((*it).first != skip)
			responder(message, it.operator*().first);
	}
}

void Channel::userLeft(User &user)
{
	for (std::map<size_t, t_user_status>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++)
	{
		if (it.operator*().first == user.get_id())
		{
			USER_MAP.erase(user.get_id());
			break;
		}
	}
}

void Channel::userBan(User &user)
{
}

void Channel::userOp(User &user)
{
}

void Channel::setOp(bool on, User *op, User *user)
{
	if (!userIsOp(*op))
		return;
	if (!user)
	{
		responder(ERR_NOSUCHNICK, *op);
		return;
	}
	t_user_status current_status = USER_MAP[user->get_id()];
	USER_MAP[user->get_id()].admin = on;
	std::string sign = on ? " +o " : " -o ";
	std::string rep = ":" + op->get_identifier() + " MODE " + _name + sign + user->get_nick();
	globalUserResponder(rep);
}

void Channel::userKick(User &user)
{
}

bool Channel::userIsOp(User &user)
{
	if (USER_MAP.find(user.get_id())->second.admin)
		return true;
	return false;
}

bool Channel::userHasVoice(User &user)
{
	if (USER_MAP.find(user.get_id())->second.voice)
		return true;
	return false;
}

t_channel_modes Channel::getModes() const
{
	return modes;
}

std::string Channel::get_creation_time()
{
	return SSTR(creation_time);
}

void Channel::setBanMask(std::string banMask, User *op)
{
	if (!userIsOp(*op))
		return;
	ban_masks.push_back(banMask);
}

void Channel::setTopic(std::string topic, User *op)
{
	if (!userIsOp(*op) && !modes.no_op_topic) 
		return;
	_topic = topic;
}

void Channel::setPrivate(bool b, User *op)
{
	if (!userIsOp(*op))
		return;
	modes.priv = b;
}

void Channel::setVoice(bool on, User *op, User *user)
{
	if (!userIsOp(*op))
		return;
	if (!user)
	{
		responder(ERR_NOSUCHNICK, *op);
		return;
	}
	t_user_status current_status = USER_MAP[user->get_id()];
	USER_MAP[user->get_id()].voice = on;
	std::string sign = on ? " +v " : " -v ";
	std::string rep = ":" + op->get_identifier() + " MODE " + _name + sign + user->get_nick();
	globalUserResponder(rep);
}

void Channel::setSecret(bool b, User *op)
{
	if (!userIsOp(*op))
		return;
	modes.secret = b;
}

void Channel::setInviteOnly(bool b, User *op)
{
	if (!userIsOp(*op))
		return;
	modes.invite = b;
}

void Channel::setNoOpTopic(bool b, User *op)
{
	if (!userIsOp(*op))
		return;
	modes.topic = b;
}

void Channel::setNoExternalMessages(bool b, User *op)
{
	if (!userIsOp(*op))
		return;
	modes.no_ext = b;
}

void Channel::setModerated(bool b, User *op)
{
	if (!userIsOp(*op))
		return;
	modes.moderate = b;
}

void Channel::setUserLimit(bool b, User *op, size_t s = 0)
{
	if (!userIsOp(*op))
		return;
	modes.has_limit = b;
	user_limit = s;
}

void Channel::setKey(bool b, User *op, std::string s = "")
{
	if (!userIsOp(*op))
		return;
	modes.has_key = b;
	_key = s;
}

#endif /* CHANNEL_HPP */
