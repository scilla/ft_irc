#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <sstream>
#include <string.h>

void replace_substr(std::string &haystack, std::string needle)
{
	size_t index = 0;
	while (true)
	{
		/* Locate the substring to replace. */
		index = haystack.find(needle.c_str(), index);
		if (index == std::string::npos)
			break;

		/* Make the replacement. */
		haystack.replace(index, needle.size(), "\n");

		/* Advance index forward so the next iteration doesn't pick it up as well. */
		index += 1;
	}
}

void print_prompt(int sig, std::string ipstr, std::string message) //sig == 0 sending; sig == 1 reciving
{
	if (!sig)
		std::cout << "[" << ipstr << "]"
				  << " << " << message << std::endl;
	else
		std::cout << "[" << ipstr << "]"
				  << " >> " << message << std::endl;
	;
}


void responder(std::string message, unsigned long fd, std::string ipstr = "127.0.0.1")
{
	static int count = 0;
	count++;
	send(fd, (message + "\n").c_str(), message.size() + 1, MSG_DONTWAIT);
	print_prompt(0, ipstr, message);
}

void responder(std::string message, User &ux)
{
	responder(message, ux.get_id(), ux.get_remote_ip());
}

std::vector<std::string> split_vct(std::string str, char delim)
{
	std::vector<std::string> tmp_vct;
	std::stringstream stream(str);
	std::string temp_str;

	while (std::getline(stream, temp_str, delim))
		tmp_vct.push_back(temp_str);
	return (tmp_vct);
}

#endif /* UTILS_HPP */
