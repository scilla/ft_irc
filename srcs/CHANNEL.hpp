#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <list>
#include "USER.hpp"

class Channel
{
	private:
		size_t _id;
		std::string _name;
		std::string psw;

		std::list<USER> USR_LIST;
	public:
		Channel();
		~Channel();

};

#endif CHANNEL_HPP