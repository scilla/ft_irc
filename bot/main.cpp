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
#include <random>
#include <time.h>
#include <fstream>
#include <pthread.h>
#define SSTR(x) static_cast<std::ostringstream &>((std::ostringstream() << std::dec << x)).str()

int SOCK;

void exit_err()
{
	std::cout << "*** ERROR ***" << std::endl;
	std::cout << "USAGE: ./ircserv <host> <port> <password>" << std::endl;
	exit(1);
}

void sender(std::string str)
{
	send(SOCK, (str + "\n").c_str(), str.size() + 1, 0);
}

std::string receiver()
{
	fd_set fds;
	int ss;
	std::string str = "";
	// FD_SET(SOCK, &fds);
	// struct timeval timeout;
	// timeout.tv_usec = 100;
	// select(SOCK + 1, &fds, NULL, NULL, &timeout);
	char buffer[102400] = {0};
	read(SOCK, buffer, 102400);
	return str;

	while ((ss = read(SOCK, buffer, 1024)) > 0)
	{
		str.append(buffer);
		usleep(1000);
		std::cout << str << "|" << ss << "|" << std::endl;
		bzero(buffer, 1024);
	}
	str.append(buffer);
	usleep(1000);
	std::cout << str << "|" << ss << "|" << std::endl;
	bzero(buffer, 1024);
	return std::string(buffer);
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

std::vector<std::string> get_channnels()
{
	sender("LIST");
	return split_vct(receiver(), '\n');
}

std::vector<std::string> load_lorem_ipsum()
{
	std::vector<std::string> res;
	std::string line;
	std::ifstream myfile("lorem_ipsum.txt");
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			res.push_back(line);
		}
		myfile.close();
	}
	else
		std::cout << "Unable to open file";
	return res;
}

int main(int ac, char **av)
{
	int pid;
	for (int i = 0; i < 100; i++) {
		sleep(1);
		pid = fork();
		if (!pid)
			break;
	}
	if (pid)
		sleep(2);
	srand(time(NULL));
	SOCK = 0;
	int valread;
	struct sockaddr_in serv_addr;
	std::vector<std::string> network;
	bool type_bool;
	std::string n_host;
	std::string n_pass;
	size_t n_port;
	std::string nick = "BoT";
	std::vector<std::string> lorem_ipsum = load_lorem_ipsum();

	for (int i = 0; i < 5; i++)
		nick.push_back("0123456789"[rand() % 10]);

	if (ac != 4)
	{
		exit_err();
	}
	check_args(av);
	n_host = av[1];
	n_pass = av[3];
	n_port = atoi(av[2]);

	
	if ((SOCK = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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
	if (connect(SOCK, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
	std::string cmd = "PASS " + n_pass + "\n";
	send(SOCK, cmd.c_str(), cmd.size(), 0);
	cmd = "NICK " + nick + "\n";
	send(SOCK, cmd.c_str(), cmd.size(), 0);
	cmd = "USER " + nick + "\n";
	send(SOCK, cmd.c_str(), cmd.size(), 0);
	std::vector<std::string> channel_to_join;
	channel_to_join.push_back("Rai1");
	channel_to_join.push_back("Rai2");
	channel_to_join.push_back("Rai3");
	channel_to_join.push_back("Rete4");
	channel_to_join.push_back("Canale5");
	channel_to_join.push_back("Italia1");
	for (std::vector<std::string>::iterator it = channel_to_join.begin(); it != channel_to_join.end(); it++)
	{
		sleep(1);
		cmd = "JOIN #" + *it;
		sender(cmd);
	}
	int i = 0;
	while (++i)
	{
		usleep(500000 + rand() % 1000000);
		cmd = "PRIVMSG #" + channel_to_join[i % channel_to_join.size()] + " :" + lorem_ipsum[rand() % lorem_ipsum.size()];
		std::cout << "Msgs sent: " << i << std::endl;
		sender(cmd);
		receiver();
	}
}