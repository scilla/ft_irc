#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <list>
#include <set>
#include "USER.hpp"
#include "errors.hpp"
#include "utils.hpp"

typedef struct s_channel_modes {
	bool priv;
	bool secret;
	bool invite;
	bool topic;
	bool no_ext;
	bool moderate;
	bool has_limit;
	bool has_key;
	bool speak;
} t_channel_modes;

typedef struct s_user_status {
	bool admin;
	bool speak;
} t_user_status;

class Channel
{
	private:
		// size_t					_id;
		std::string						_name;
		std::string						_key;
		std::string						_topic;
		std::map<size_t, t_user_status>	USER_MAP; //mappa con user right come key e utente corrispondende
		std::set<size_t>				invited_users;
		size_t							user_limit;
		t_channel_modes					modes;
		std::vector<std::string>		ban_masks;
	public:
		Channel(std::string);
		~Channel();

		void userJoin(User&, std::string);
		void userLeft(User&);
		void userBan(User&);
		void userOp(User&);
		void userKick(User&);

		t_channel_modes getModes() const;
		void setModes(t_channel_modes);
		void setTopic(std::string);
		void globalUserResponder(std::string, size_t);

		void setOp(bool, std::string);
		void setPrivate(bool);
		void setSecret(bool);
		void setInviteOnly(bool);
		void setNoOpTopic(bool);
		void setNoExternalMessages(bool);
		void setModerated(bool);
		void setSpeak(bool, std::string);
		void setUserLimit(bool, size_t);
		void setKey(bool, std::string);
		void setBanMask(std::string);
		std::string	get_user_nb();
		std::string get_modes_str();
		std::string get_topic();
		std::string get_name();
		std::vector<size_t> get_users_ids();
};

Channel::Channel(std::string channel_name) {
	_name = channel_name;
	_key = "";
	_topic = "";
	user_limit = 0;
	modes = (t_channel_modes){false, false, false, false, false, false, false};
};

Channel::~Channel() {};

std::string		Channel::get_user_nb()
{
	std::stringstream connectedUsersNb;
	connectedUsersNb << USER_MAP.size();
	std::string number = connectedUsersNb.str();
	return(number);
}

std::string Channel::get_name()
{
	return(_name);
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

std::string Channel::get_modes_str()
{
	std::string res = "[+";
	if(modes.has_key)
		res.append("k");
	if(modes.has_limit)
		res.append("l");
	if(modes.invite)
		res.append("i");
	if(modes.moderate)
		res.append("m");
	if(modes.no_ext)
		res.append("n");
	if(modes.priv)
		res.append("p");
	if(modes.secret)
		res.append("s");
	if(modes.topic)
		res.append("t");
	res.append("]");
	return(res);
}

std::string Channel::get_topic(){ return(_topic); }

std::vector<size_t> Channel::get_users_ids()
{
	std::vector<size_t> res;
	for(std::map<size_t, t_user_status>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++)
		res.push_back((*it).first);
	return(res);
}


void Channel::userJoin(User& user, std::string pass = "") {
	t_user_status stat((t_user_status){false, false});
	if (modes.has_key && pass != _key) {
		responder(ERR_BADCHANNELKEY, user);
		return;
	}
	if (modes.has_limit && USER_MAP.size() >= user_limit) {
		responder(ERR_CHANNELISFULL, user);
		return;
	}
	if (modes.invite) {
		if (!invited_users.count(user.get_id())) {
			responder(ERR_INVITEONLYCHAN, user);
			return;
		}
		invited_users.erase(user.get_id()); // todo: delete fd when disconnected
	}
	if(USER_MAP.find(user.get_id()) != USER_MAP.end())
	{
		responder(ERR_ALREADYREGISTRED, user);
		return;
	}
	if (!USER_MAP.size())
		stat.admin = true;
	std::pair<std::map<size_t, t_user_status>::iterator, bool> ritorno = USER_MAP.insert(std::pair<size_t, t_user_status>(user.get_id(), stat));
	std::cout << "RITORNO ID: " << ritorno.first.operator*().first << " IS_NEW: " << ritorno.second << std::endl;
	std::cout << "User " << user.get_nick() << " joined channel " << _name << std::endl;
	for(std::map<size_t, t_user_status>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++) //communicate to all the user connected to the same channel that the new user connected
	{
		responder(":" + user.get_identifier() + " JOIN " + this->_name, (*it).first);
		std::cout << "[SENT JOIN MESSAGE TO]: " << (*it).first << " Size: " <<USER_MAP.size() << std::endl;
	}
	// todo: broadcast join to users already in channel
	// todo: RPL_TOPIC
	// todo: RPL_NAMREPLY list users including me
}

void Channel::globalUserResponder(std::string message, size_t skip = 0)
{
	for(std::map<size_t, t_user_status>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++){
		if((*it).first != skip)
			responder(message, it.operator*().first);
	}
}

void Channel::userLeft(User& user) {

	for(std::map<size_t, t_user_status>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++) 
	{
		if(it.operator*().first == user.get_id())
		{
			USER_MAP.erase(user.get_id());
			break;
		}
	}
}

void Channel::userBan(User& user) {

}

void Channel::userOp(User& user) {

}

void Channel::userKick(User& user) {

}

t_channel_modes Channel::getModes() const {
	return modes;
}

void Channel::setBanMask(std::string banMask) {
	ban_masks.push_back(banMask);
}

void Channel::setModes(t_channel_modes new_modes) {
	modes = new_modes;
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}


void Channel::setPrivate(bool b = true) {
	modes.priv = b;
}

void Channel::setSpeak(bool b = true, std::string nick = "") {
	modes.speak = b;
}

void Channel::setSecret(bool b = true) {
	modes.secret = b;
}

void Channel::setInviteOnly(bool b = true) {
	modes.invite = b;
}

void Channel::setNoOpTopic(bool b = true) {
	modes.topic = b;
}

void Channel::setNoExternalMessages(bool b = true) {
	modes.no_ext = b;
}

void Channel::setModerated(bool b = true) {
	modes.moderate = b;
}

void Channel::setUserLimit(bool b = true, size_t s = 0) {
	modes.has_limit = b;
	user_limit = s;
}

void Channel::setKey(bool b = true, std::string s = "") {
	modes.has_key = b;
	_key = s;
}


#endif /* CHANNEL_HPP */
