#pragma once
#include "../socket/BindingSocket.hpp"
#include "../socket/ConnectingSocket.hpp"
#include "../socket/ListeningSocket.hpp"
#include "../socket/Socket.hpp"

class Server
{
private:
    ListeningSocket * socket;
    virtual void accepter() = 0;
    virtual void handler() = 0;
    virtual void responder() = 0;
public:
    Server();
    Server(int domain, int service, int protocol, int serverPort, u_long interface, int backlog);
    ~Server();
    virtual void launch() = 0;
    ListeningSocket * getServerSocket();
};

Server::Server(){}

Server::Server(int domain, int service, int protocol, int serverPort, u_long interface, int backlog)
{
	socket = new ListeningSocket(domain, service, protocol, serverPort, interface, backlog);
}
	
Server::~Server(){
	delete socket;
}

ListeningSocket * Server::getServerSocket(){
	return socket;
}