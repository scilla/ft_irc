#include "BindingSocket.hpp"

FT::BindingSocket::BindingSocket(int domain, int service, int protocol, int serverPort, u_long interface):
    FT::Socket(domain, service, protocol, serverPort, interface){
        setConnection(netConnection(getSocket(), getRemote()));
        testConnection(getConnection());
    }
    
FT::BindingSocket::~BindingSocket(){}

int FT::BindingSocket::netConnection(int hSocket, struct sockaddr_in remote){
    return bind(hSocket, (struct sockaddr *)&remote, sizeof(remote));
}