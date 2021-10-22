#include "ConnectingSocket.hpp"

FT::ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int serverPort, u_long interface):
    FT::Socket(domain, service, protocol, serverPort, interface){
        setConnection(netConnection(getSocket(), getRemote()));
        testConnection(getConnection());
    }
    
FT::ConnectingSocket::~ConnectingSocket(){}

int FT::ConnectingSocket::netConnection(int hSocket, struct sockaddr_in remote){
    return connect(hSocket, (struct sockaddr *)&remote, sizeof(remote));
}