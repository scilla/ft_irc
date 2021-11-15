#include <vector>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <sys/select.h>
#include "commands.hpp"

void exit_err()
{
	std::cout << "*** ERROR ***" << std::endl << "USAGE: ./ircserv [host:port_network:password_network] <port> <password>" << std::endl;
	std::cout << "USAGE: ./ircserv <port> <password>" << std::endl;
	exit (1);
}

bool isNumber(const char *str)
{
    for (int i = 0; str[i]; i++) {
        if (std::isdigit(str[i]) == 0)
			return false;
    }
    return true;
}

void check_args(char **av)
{
	std::vector<std::string> tmp;
	tmp = split_vct(av[1], ':');
	if(tmp.size() != 3)
		exit_err();
	else if(!isNumber(tmp[1].c_str()))
		exit_err();
	else if(!isNumber(av[2]))
		exit_err();
}


int main(int ac, char **av)
{
	std::vector<std::string> network;
	bool type_bool;
	std::string n_host;
	size_t 		n_port;
	std::string n_psw;
	size_t		l_port;
	std::string	l_psw;

	if(ac != 3 && ac != 4) {
		exit_err();
		return 0;
	}
	if(ac == 4)
	{
		check_args(av);
		network = split_vct(av[1], ':');
		network.push_back(av[2]);
		network.push_back(av[3]);
		n_host = (std::string)network[0].c_str();
		n_port = atol(network[1].c_str());
		n_psw = (std::string)network[2].c_str();
		l_port = atol(network[3].c_str());
		l_psw = (std::string)network[4].c_str();
		type_bool = false;
		//IRC server((std::string)network[0].c_str(), atol(network[1].c_str()), (std::string)network[2].c_str(), atol(network[3].c_str()), (std::string)network[4].c_str(), false);
	} 
	else
	{
		n_host = "OWN";
		n_port = -1;
		n_psw = "OWN";
		l_port = atol(av[1]);
		l_psw = av[2];
		type_bool = true;
		if(!isNumber(av[1])) {
			exit_err();
			return 0;
		}
	}

	if (l_port < 1024 || l_port > 65535) {
		printf("\n Port out of range [1024-65535] \n");
		return -1;
	}
	IRC server(n_host, n_port, n_psw, l_port, l_psw, type_bool);
	server.launch();

	//debug
	//std::cout << "host				: " << std::setw(10) << network[0] << std::endl;
	//std::cout << "port_network			: " << std::setw(10) << network[1] << std::endl;
	//std::cout << "password_network		: " << std::setw(10) << network[2] << std::endl;
	//std::cout << "port				: " << std::setw(10) << network[3] << std::endl;
	//std::cout << "password			: " << std::setw(10) << network[4] << std::endl;
}