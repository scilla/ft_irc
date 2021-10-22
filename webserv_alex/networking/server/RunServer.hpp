#pragma once

#include <unistd.h>
#include "Server.hpp"

namespace FT{
    class RunServer: public Server
    {
    private:
        char buff[30000];
        int newSocket;
        void accepter();
        void handler();
        void responder();
    public:
        RunServer();
        ~RunServer();
        void launch();
    };
    
    
    
}