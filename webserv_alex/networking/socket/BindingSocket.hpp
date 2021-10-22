#pragma once
#include "Socket.hpp"

namespace FT{
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
    
    
    
}