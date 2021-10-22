#pragma once

#include "BindingSocket.hpp"

namespace FT{
    
    class ListeningSocket: public BindingSocket
    {

    private:
        int backlog;
        int listening;
    public:
        ListeningSocket(int domain, int service, int protocol, int serverPort, u_long interface, int backlog);
        ~ListeningSocket();

        void startListening();

        int getListening();
        int getBacklog();

    };
}

