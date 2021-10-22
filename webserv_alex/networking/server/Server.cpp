#include "Server.hpp"

FT::Server::Server(){}

FT::Server::Server(int domain, int service, int protocol, int serverPort, u_long interface, int backlog)
{
    socket = new ListeningSocket(domain, service, protocol, serverPort, interface, backlog);
    }
    
FT::Server::~Server(){
    delete socket;
    }

FT::ListeningSocket * FT::Server::getServerSocket(){
    return socket;
}