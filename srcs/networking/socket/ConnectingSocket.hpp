#pragma once
#include "Socket.hpp"

class ConnectingSocket: public Socket
{
public:
	ConnectingSocket(int domain, int service, int protocol, int serverPort, u_long interface);
	~ConnectingSocket();
	int netConnection(int hSocket, struct sockaddr_in remote);
};


ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int serverPort, u_long interface):
	Socket(domain, service, protocol, serverPort, interface){
		setConnection(netConnection(getSocket(), getRemote()));
		if(testConnection(getConnection()))
			exit(EXIT_FAILURE);
	}
	
ConnectingSocket::~ConnectingSocket(){}

int ConnectingSocket::netConnection(int hSocket, struct sockaddr_in remote){
	return connect(hSocket, (struct sockaddr *)&remote, sizeof(remote));
}