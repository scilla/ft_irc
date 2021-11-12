#include <vector>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

void exit_err()
{
	std::cout << "*** ERROR ***" << std::endl;
	std::cout << "USAGE: ./ircserv <host> <port> <password>" << std::endl;
	exit(1);
}

bool isNumber(const char *str)
{
	for (int i = 0; str[i]; i++)
	{
		if (std::isdigit(str[i]) == 0)
			return false;
	}
	return true;
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

void check_args(char **av)
{
	if (!isNumber(av[2]))
		exit_err();
}

int main(int ac, char **av)
{
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	std::vector<std::string> network;
	bool type_bool;
	std::string n_host;
	std::string n_pass;
	size_t n_port;
	std::string hello("Hello from client");
	char buffer[10240] = {0};

	if (ac != 4)
	{
		exit_err();
	}
	check_args(av);
	n_host = av[1];
	n_pass = av[3];
	n_port = atoi(av[2]);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(n_port);

	if (inet_pton(AF_INET, n_host.c_str(), &serv_addr.sin_addr) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
	hello = "PASS " + n_pass + "\n";
	send(sock, hello.c_str(), hello.size(), 0);
	hello = "NICK stocazzo\n";
	send(sock, hello.c_str(), hello.size(), 0);
	hello = "USER stocazzo\n";
	send(sock, hello.c_str(), hello.size(), 0);
	hello = "LIST\n";
	send(sock, hello.c_str(), hello.size(), 0);
    valread = read( sock , buffer, 10240);
    printf("%s\n",buffer );
	hello = "LIST\n";
	send(sock, hello.c_str(), hello.size(), 0);
    valread = read( sock , buffer, 10240);
    printf("%s\n",buffer );
}