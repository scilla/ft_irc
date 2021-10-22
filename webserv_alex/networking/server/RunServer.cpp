#include "RunServer.hpp"



FT::RunServer::RunServer(): Server(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10)
    {
        launch();
    }
    
FT::RunServer::~RunServer()
    {
    }

void FT::RunServer::accepter(){
    struct sockaddr_in remote = getServerSocket()->getRemote();
    int remoteLen = sizeof(remote);
    newSocket = accept(getServerSocket()->getSocket(), (struct sockaddr *)&remote, (socklen_t * )&remoteLen);
    read(newSocket, buff, sizeof(buff));

}

void FT::RunServer::handler(){
    std::cout<< buff << std::endl;
}

void FT::RunServer::responder(){

    std::string message("Hello from server");
    write(newSocket, message.c_str(), message.length());
    close(newSocket);
}

void FT::RunServer::launch(){
    while(true){
        std::cout << "------------WAITING-------------" << std::endl;
        accepter();
        handler();
        responder();
        std::cout << "------------DONE-----------" << std::endl;

    }
}
