#pragma once
#include "Socket.hpp"

class BindingSocket: public Socket
{
private:
    int binding;
public:
    BindingSocket(int domain, int service, int protocol, int serverPort, u_long interface);
    ~BindingSocket();
    int netConnection(int hSocket, struct sockaddr_in remote);
    int getBinding();
};

BindingSocket::BindingSocket(int domain, int service, int protocol, int serverPort, u_long interface):
	Socket(domain, service, protocol, serverPort, interface){
		while(true){
            setConnection(netConnection(getSocket(), getRemote()));
		    if (getConnection() < 0){
                std::cout << "retry" << std::endl;
                sleep(3); 
                continue;
            }
            else{
                break;
            }
        }
        std::cout << "connected" << std::endl;
        // perror("ok\n");
	}
	
BindingSocket::~BindingSocket(){}

int BindingSocket::netConnection(int hSocket, struct sockaddr_in remote){
	return bind(hSocket, (struct sockaddr *)&remote, sizeof(remote));
}


