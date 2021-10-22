#pragma once

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

namespace FT
{
    class Socket
    {
        private:
            int serverPort;
            int hSocket;
            int connection;
            struct sockaddr_in remote;
            u_long interface;
        public:
            Socket();
            Socket(int domain, int service, int protocol, int serverPort, u_long interface);
            virtual int netConnection(int hSocket, struct sockaddr_in remote) = 0;
            void testConnection(int toTest);

            int getSocket();
            int getConnection(); // to check!!!

            void setConnection(int con);
            struct sockaddr_in getRemote();
    };
}

