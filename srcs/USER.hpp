#ifndef USER_HPP
#define USER_HPP

#include <iostream>

class USER
{
	private:
		size_t _id;
		std::string _nick; //max 9 chars
		std::string _psw;
	public:
		USER(size_t id, std::string nick, std::string psw);
		~USER();
		std::string get_nick();
		std::string get_psw();
		void set_nick(std::string nick);
		void set_pwd(std::string psw);
};

#endif