
#pragma once

#include <unistd.h>
#include "Server.hpp"


class RunLocal: public Server
{
private:
	char buff[500];
	int newSocket;
	void accepter();
	void handler();
	void responder();
	std::string psw;
public:
	RunLocal(size_t port, std::string psw);
	~RunLocal();
	void launch();
};


RunLocal::RunLocal(size_t port, std::string psw): Server(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY, 10496) {
	this->psw = psw;
	launch();
}
	
RunLocal::~RunLocal() {}

void RunLocal::accepter(){
	struct sockaddr_in remote = getServerSocket()->getRemote();
	int remoteLen = sizeof(remote);
	
	newSocket = accept(getServerSocket()->getSocket(), (struct sockaddr *)&remote, (socklen_t * )&remoteLen);
	read(newSocket, buff, sizeof(buff));

}

void RunLocal::handler(){
	// std::cout<< buff << std::endl;
}

void RunLocal::responder(){

	std::string message("Hello from server");
	write(newSocket, message.c_str(), message.length());
	//close(newSocket);
}

void RunLocal::launch(){
	while(true){
		std::cout << "------------WAITING-------------" << std::endl;
		accepter();
		handler();
		responder();
		std::cout << "------------DONE-----------" << std::endl;

	}
}