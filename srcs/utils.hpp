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

void print_prompt(int sig, std::string message) //sig == 0 sending; sig == 1 reciving
{
	char str[INET_ADDRSTRLEN];
	struct hostent *hp;
	//getnameinfo( (struct sockaddr *)&remote /* type cast sockaddr_in to sockaddr */, sizeof(remote), str, 64, NULL, 0, 0); //get the hostname non so se mantenerlo o meno
	
	if(!sig)
		std::cout << "[" << inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN ) << "]" << " << " << message << std::endl;
	else
		std::cout << "[" << inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN ) << "]" << " >> " << message << std::endl;;
}

void responder(std::string message, int fd) {
	write(fd, message.c_str(), message.length());
	write(fd, "\n", 1);
	print_prompt(0, message);
}

#endif /* UTILS_HPP */
