#pragma once

#include "BindingSocket.hpp"
class ListeningSocket: public BindingSocket
{

	private:
	    int backlog;
	    int listening;
	public:
	    ListeningSocket(int domain, int service, int protocol, int serverPort, u_long interface, int backlog);
	    virtual ~ListeningSocket();
	    void startListening();
	    int getListening();
	    int getBacklog();
};

ListeningSocket::ListeningSocket(int domain, int service, int protocol, int serverPort, u_long interface, int backlog):
	BindingSocket(domain, service, protocol, serverPort, interface)
{
	this->backlog = backlog;
	startListening();
	testConnection(this->listening);
}

ListeningSocket::~ListeningSocket()
{
}

void ListeningSocket::startListening(){
	this->listening = listen(getSocket(), this->listening);
	
}

int ListeningSocket::getListening(){
	return listening;
}

int ListeningSocket::getBacklog(){
	return this->backlog;
}
