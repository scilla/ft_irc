#pragma once

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>


class Socket
{
    private:
        int serverPort;
        int hSocket;
        int connection;
        struct sockaddr_in remote;
        u_long interface;
    public:
        Socket();
		~Socket() {};
        Socket(int domain, int service, int protocol, int serverPort, u_long interface);
        virtual int netConnection(int hSocket, struct sockaddr_in remote) = 0;
        void testConnection(int toTest);
        int getSocket();
        int getConnection(); // to check!!!
        void setConnection(int con);
        struct sockaddr_in getRemote();
};

Socket::Socket(){}

Socket::Socket(int domain, int service, int protocol, int serverPort, u_long interface){
	std::cout << "Create the socket" << std::endl;
	// The htons function converts a u_short from host to TCP/IP network byte order (which is big-endian).
	this->remote.sin_family = domain;
	this->remote.sin_port = htons(serverPort);
	this->remote.sin_addr.s_addr = htonl(interface);
	// socket(AF_INET, SOCK_STREAM, 0);
	this->hSocket = socket(domain, service, protocol);
	this->testConnection(hSocket);
}

void Socket::testConnection(int toTest){
	if(toTest < 0){
		perror("Failed to connect...");
		exit(EXIT_FAILURE);
	}
}

int Socket::getSocket(){
	return hSocket;
}

int Socket::getConnection(){
	return connection;
}


void Socket::setConnection(int con){
	connection = con;
}

struct sockaddr_in Socket::getRemote(){
	return remote;
}
