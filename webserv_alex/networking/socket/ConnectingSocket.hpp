#pragma once
#include "Socket.hpp"

namespace FT{
    class ConnectingSocket: public Socket
    {
    public:
        ConnectingSocket(int domain, int service, int protocol, int serverPort, u_long interface);
        ~ConnectingSocket();

        int netConnection(int hSocket, struct sockaddr_in remote);
    };
    
    
    
}