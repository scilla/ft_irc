#include "ListeningSocket.hpp"

FT::ListeningSocket::ListeningSocket(int domain, int service, int protocol, int serverPort, u_long interface, int backlog):
    BindingSocket(domain, service, protocol, serverPort, interface)
{
    this->backlog = backlog;
    startListening();
    testConnection(this->listening);
}

FT::ListeningSocket::~ListeningSocket()
{
}

void FT::ListeningSocket::startListening(){
    this->listening = listen(getSocket(), this->listening);
    
}

int FT::ListeningSocket::getListening(){
    return listening;
}

int FT::ListeningSocket::getBacklog(){
    return this->backlog;
}