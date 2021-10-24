#pragma once

#include <unistd.h>
#include "Server.hpp"


class RunServer: public Server
{
private:
	char buff[30000];
	int newSocket;
	void accepter();
	void handler();
	void responder();
public:
	RunServer();
	~RunServer();
	void launch();
};


RunServer::RunServer(): Server(AF_INET, SOCK_STREAM, 0, 8081, INADDR_ANY, 10) {
	launch();
}
	
RunServer::~RunServer() {}

void RunServer::accepter(){

	struct sockaddr_in remote = getServerSocket()->getRemote();
	int remoteLen = sizeof(remote);
	newSocket = accept(getServerSocket()->getSocket(), (struct sockaddr *)&remote, (socklen_t * )&remoteLen);
	while(recv(newSocket, buff, 800000, 0)){
		handler();
		//responder();
		bzero(buff, sizeof(buff));
	}

}

void RunServer::handler(){
	std::cout<< buff << std::endl;
}

void RunServer::responder(){

	std::string message("CAP * LS :");
	write(newSocket, message.c_str(), message.length());
	//close(newSocket);
}

void RunServer::launch(){
	while(true){
		std::cout << "------------WAITING-------------" << std::endl;
		accepter();
		//handler();
		//responder();
		std::cout << "------------DONE-----------" << std::endl;

	}
}