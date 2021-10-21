#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <list>
#include "USER.hpp"

class CHANNEL
{
	private:
		size_t _id;
		std::string _name;
		std::string psw;

		std::list<USER> USR_LIST;
	public:
		CHANNEL();
		~CHANNEL();

};

#endif CHANNEL_HPP