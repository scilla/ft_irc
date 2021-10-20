#include "main.hpp"
#include <vector>
#include <iostream>
#include <sstream>

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


int main(int ac, char **av)
{
	std::vector<std::string> network;

	if(ac != 3 && ac != 4)
	{
		// error wrong arg
	}
	if(ac == 4)
	{
		network = split_vct(av[1], ':');
		network.push_back(av[2]);
		network.push_back(av[3]);
		IRC context((std::string)network[0].c_str(), atol(network[1].c_str()), (std::string)network[2].c_str(), atol(network[3].c_str()), (std::string)network[4].c_str(), false);
	}
	else
	{
		network.push_back(av[1]);
		network.push_back(av[2]);
		IRC context(0, 0, 0, atol(network[0].c_str()), (std::string)network[1].c_str(), true);
	}

	std::cout << "host				: " << network[0] << std::endl;
	std::cout << "port_network		: " << network[1] << std::endl;
	std::cout << "password_network	: " << network[2] << std::endl;
	std::cout << "port				: " << network[3] << std::endl;
	std::cout << "password			: " << network[4] << std::endl;


}