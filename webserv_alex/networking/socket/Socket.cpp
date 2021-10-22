#include "Socket.hpp"

FT::Socket::Socket(){}

FT::Socket::Socket(int domain, int service, int protocol, int serverPort, u_long interface){
    std::cout << "Create the socket" << std::endl;
    // The htons function converts a u_short from host to TCP/IP network byte order (which is big-endian).
    this->remote.sin_family = domain;
    this->remote.sin_port = htons(serverPort);
    this->remote.sin_addr.s_addr = htonl(interface);
    // socket(AF_INET, SOCK_STREAM, 0);
    this->hSocket = socket(domain, service, protocol);
    this->testConnection(hSocket);
}

void FT::Socket::testConnection(int toTest){
    if(toTest < 0){
        perror("Failed to connect...");
        exit(EXIT_FAILURE);
    }
}

int FT::Socket::getSocket(){
    return hSocket;
}

int FT::Socket::getConnection(){
    return connection;
}


void FT::Socket::setConnection(int con){
    connection = con;
}

struct sockaddr_in FT::Socket::getRemote(){
    return remote;
}