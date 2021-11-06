#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <list>
#include <set>
#include "USER.hpp"
#include "ERRORS.hpp"

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
		int								user_limit;
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


void Channel::userJoin(User& user, std::string pass = ""){
	if (modes.has_key && pass != key) {
		responder(ERR_BADCHANNELKEY, user.get_id());
	}
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
