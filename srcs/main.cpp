#include "main.hpp"

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
		network[3] = av[2];
		network[4] = av[3];
		IRC context((std::string)network[0].c_str(), atol(network[1].c_str()), (std::string)network[2].c_str(), atol(network[3].c_str()), (std::string)network[4].c_str(), false);
	}
	else
	{
		network[0] = av[1];
		network[1] = av[2];
		IRC context(0, 0, 0, atol(network[0].c_str()), (std::string)network[1].c_str(), true);
	}


}