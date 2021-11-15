#pragma once
// #include "../socket/BindingSocket.hpp"
// #include "../socket/ConnectingSocket.hpp"
#include "../socket/ListeningSocket.hpp"
#include "../socket/Socket.hpp"

class Server
{
private:
    virtual void accepter() = 0;
    virtual void handler(int) = 0;
    //virtual void responder(std::string, int) = 0;
protected:
    ListeningSocket * Socket;
public:
    Server();
    Server(int domain, int service, int protocol, int serverPort, u_long interface, int backlog);
   	virtual ~Server();
    virtual void launch() = 0;
    ListeningSocket * getServerSocket();
};

Server::Server(){}

Server::Server(int domain, int service, int protocol, int serverPort, u_long interface, int backlog)
{
	Socket = new ListeningSocket(domain, service, protocol, serverPort, interface, backlog);
}
	
Server::~Server(){
	delete Socket;
}

ListeningSocket * Server::getServerSocket(){
	return Socket;
}