#include "main.hpp"
#include <vector>
#include <iostream>
#include <sstream>

void exit_err()
{
	std::cout << "*** ERROR ***" << std::endl << "USAGE: ./ircserv [host:port_network:password_network] <port> <password>" << std::endl;
	std::cout << "USAGE: ./ircserv <port> <password>" << std::endl;
	exit (-1);
}

std::vector<std::string> split_vct(std::string str, char delim)
{
	std::vector<std::string> tmp_vct;
	std::stringstream stream(str);
	std::string temp_str;
	
	while(std::getline(stream, temp_str, delim))
	{
		tmp_vct.push_back(temp_str);
	}
	return (tmp_vct);
}

void check_args(char **av)
{
	std::vector<std::string> tmp;
	tmp = split_vct(av[1], ':');
	if(tmp.size() != 3)
		exit_err();
	else if(!atol(tmp[1].c_str()))
		exit_err();
	else if(!atol(av[2]))
		exit_err();
}


int main(int ac, char **av)
{
	std::vector<std::string> network;

	if(ac != 3 && ac != 4)
		exit_err();
	if(ac == 4)
	{
		check_args(av);
		network = split_vct(av[1], ':');
		network.push_back(av[2]);
		network.push_back(av[3]);
		IRC context((std::string)network[0].c_str(), atol(network[1].c_str()), (std::string)network[2].c_str(), atol(network[3].c_str()), (std::string)network[4].c_str(), false);
	}
	else
	{
		if(!atol(av[1]))
			exit_err();
		IRC context(0, 0, 0, atol(av[1]), av[2], true);
	}

	std::cout << "host				: " << network[0] << std::endl;
	std::cout << "port_network		: " << network[1] << std::endl;
	std::cout << "password_network	: " << network[2] << std::endl;
	std::cout << "port				: " << network[3] << std::endl;
	std::cout << "password			: " << network[4] << std::endl;


}