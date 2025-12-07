#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

int main(){
    int clinetSocket = socket(AF_INET, SOCK_STREAM, 0);


    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int con = connect(clinetSocket, (struct sockaddr*)& serverAddress, sizeof(serverAddress));

    if(con == -1){ std::cerr<< " something went wrong with the connection: " << strerror(errno)<<std::endl; exit(-1);}

    while(true){
        std::string clientMessage;
        std::cout<<"Enter message: ";
        std::getline(std::cin, clientMessage);

        if (clientMessage == "exit") break;

        send(clinetSocket, clientMessage.c_str(), clientMessage.length(), 0);
    }

    close(clinetSocket);
    return 0;
}
