#ifndef CONNECTINGSOCKET_HPP
#define CONNECTINGSOCKET_HPP
#include "Socket.hpp"

class ConnectingSocket: public Socket
{
public:
	ConnectingSocket(int domain, int service, int protocol, int serverPort, u_long interface, char *ip_addr);
	~ConnectingSocket();
	int netConnection(int hSocket, struct sockaddr_in remote);
};


ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int serverPort, u_long interface, char *ip_addr):
	Socket(domain, service, protocol, serverPort, interface, ip_addr){
		//sockaddr_in sa;
		//sa.sin_port = htons(serverPort);
		//sa.sin_family = domain;
		//unsigned char remote_buff[500];
		//sa.sin_zero = remote_buff;
		//int a = inet_pton(AF_INET, ip_addr, &(sa.sin_addr));
		//sa.sin_family = domain;
		//std::cout << ip_addr << std::endl;
		//struct sockaddr_in in;
		//std::cout << sa.sin_addr.s_addr << std::endl;
		//char *tmp = inet_ntoa(sa.sin_addr);
		//std::cout << tmp << std::endl;
		struct sockaddr *j = (struct sockaddr *)&getRemote();
		setConnection(connect(getSocket(), j, sizeof(getRemote())));
		if(testConnection(getConnection()))
			exit(EXIT_FAILURE);
	}
	
ConnectingSocket::~ConnectingSocket(){}

int ConnectingSocket::netConnection(int hSocket, struct sockaddr_in remote){
	return connect(hSocket, (struct sockaddr *)&remote, sizeof(remote));
}


#endif /* CONNECTINGSOCKET_HPP */
