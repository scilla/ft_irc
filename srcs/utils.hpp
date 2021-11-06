#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <sstream>
 #include <string.h>


void replace_substr(std::string& haystack, std::string needle)
{
	size_t index = 0;
	while (true) {
		/* Locate the substring to replace. */
		index = haystack.find(needle.c_str(), index);
		if (index == std::string::npos) break;

		/* Make the replacement. */
		haystack.replace(index, needle.size(), "\n");

		/* Advance index forward so the next iteration doesn't pick it up as well. */
		index += 1;
	}
}

void print_prompt(int sig, std::string ipstr, std::string message) //sig == 0 sending; sig == 1 reciving
{
	char str[INET_ADDRSTRLEN];
	char *ipstr_c = const_cast<char*>(ipstr.c_str());
	//getnameinfo( (struct sockaddr *)&remote /* type cast sockaddr_in to sockaddr */, sizeof(remote), str, 64, NULL, 0, 0); //get the hostname non so se mantenerlo o meno
	
	if(!sig)
		std::cout << "[" << inet_ntop( AF_INET, ipstr_c, str, INET_ADDRSTRLEN ) << "]" << " << " << message << std::endl;
	else
		std::cout << "[" << inet_ntop( AF_INET, ipstr_c, str, INET_ADDRSTRLEN ) << "]" << " >> " << message << std::endl;;
}

void responder(std::string message, User& ux) {
	write(ux.get_id(), message.c_str(), message.length());
	write(ux.get_id(), "\n", 1);
	print_prompt(0, ux.get_ip_str(), message);
}

#endif /* UTILS_HPP */
