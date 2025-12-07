
#include "../include/handelclient.hpp"

int main(){
    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    HandelClient* Hc = new HandelClient(serverSocket);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;


    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 5);

    Hc->run();


    close(serverSocket);
    delete Hc;
    return 0;
}
