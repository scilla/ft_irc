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
		size_t							user_count;
		t_channel_modes					modes;
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
		void setKey(std::string);
		void setTopic(std::string);
};

Channel::Channel(std::string channel_name) {
	_name = channel_name;
	modes = (t_channel_modes){false, false, false, false, false, false, false};
};

Channel::~Channel() {};

void Channel::userJoin(User& user, std::string pass = "") {
	t_user_status stat((t_user_status){false, false});
	if (modes.has_key && pass != key) {
		responder(ERR_BADCHANNELKEY, user.get_id());
		return;
	}
	if (modes.has_limit && USER_MAP.size() >= user_limit) {
		responder(ERR_CHANNELISFULL, user.get_id());
		return;
	}
	if (modes.invite) {
		if (!invited_users.count(user.get_id())) {
			responder(ERR_INVITEONLYCHAN, user.get_id());
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
		responder(":" + user.get_nick() + "!" + user.get_username() + "@" + user.get_host() + user.get_domain(), it.operator*().first);
	}

	// todo: broadcast join to users already in channel
	// todo: RPL_TOPIC
	// todo: RPL_NAMREPLY list users including me
}

void Channel::userLeft(User& user){

}

void Channel::userBan(User& user){

}

void Channel::userOp(User& user){

}

void Channel::userKick(User& user){

}

t_channel_modes Channel::getModes() const{

}

void Channel::setModes(t_channel_modes){

}

void Channel::setKey(std::string){

}


#endif /* CHANNEL_HPP */
