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
} t_channel_modes;

typedef struct s_user_status {
	bool admin;
	bool banned;
} t_user_status;

class Channel
{
	private:
		// size_t					_id;
		std::string						_name;
		std::string						key;
		std::string						topic;
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
		void globalUserResponder(std::string);

		void setPrivate(bool);
		void setSecret(bool);
		void setInviteOnly(bool);
		void setNoOpTopic(bool);
		void setNoExternalMessages(bool);
		void setModerated(bool);
		void setUserLimit(bool, size_t);
		void setKey(bool, std::string);
};

Channel::Channel(std::string channel_name) {
	_name = channel_name;
	key = "";
	topic = "";
	user_limit = 0;
	modes = (t_channel_modes){false, false, false, false, false, false, false};
};

Channel::~Channel() {};

void Channel::userJoin(User& user, std::string pass = "") {
	t_user_status stat((t_user_status){false, false});
	if (modes.has_key && pass != key) {
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
		invited_users.erase(user.get_id());
	}
	if (!USER_MAP.size())
		stat.admin = true;
	USER_MAP.insert(std::make_pair(user.get_id(), stat));
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

void Channel::globalUserResponder(std::string message)
{
	for(std::map<size_t, t_user_status>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++) //communicate to all the user connected to the same channel that the new user connected
	{
		responder(message, it.operator*().first);
	}
}

void Channel::userLeft(User& user){
	for(std::map<size_t, t_user_status>::iterator it = USER_MAP.begin(); it != USER_MAP.end(); it++) //communicate to all the user connected to the same channel that the new user connected
	{
		if(it.operator*().first == user.get_id())
		{
			USER_MAP.erase(user.get_id());
			globalUserResponder(user.get_nick() + " left the channel");
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

void Channel::setModes(t_channel_modes new_modes) {
	modes = new_modes;
}

void Channel::setPrivate(bool b = true) {
	modes.priv = b;
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
	key = s;
}


#endif /* CHANNEL_HPP */
