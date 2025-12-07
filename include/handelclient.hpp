#ifndef HANDEL_CLIENT
#define HANDEL_CLINET

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>


class HandelClient{

    private:
    int serverSocket;
    fd_set masterSet, readSet;
    int maxFD;

    public:
        HandelClient(const int serverSocket): serverSocket(serverSocket) {
            FD_ZERO(&masterSet);
            FD_ZERO(&readSet);
            FD_SET(serverSocket, &masterSet);
        }
     
        void run(){
           
            int maxFD = serverSocket;
            while(true){
                readSet = masterSet;
                int activity = select(maxFD + 1 , &readSet, nullptr, nullptr, nullptr);

                if(activity < 0){
                    perror("select error");
                    continue;
                }

                //Check for new clinet
                if(FD_ISSET(serverSocket, &readSet)){
                    int clientSocket = accept(serverSocket, nullptr, nullptr);
                    std::cout << "New client connected: " << clientSocket << "\n";

                    FD_SET(clientSocket, &masterSet);
                    if(clientSocket > maxFD) maxFD = clientSocket;

                }
            

            //Handle existing clinets
                for(int sock = 0; sock <= maxFD; sock++){
                    if(sock != serverSocket && FD_ISSET(sock, &readSet)){
                        char buffer[1024];
                        int bytes = recv(sock, buffer, sizeof(buffer), 0);

                        if(bytes <= 0){
                            std::cout << "Client Disconnect: " << sock << "\n";
                            close(sock);
                            FD_CLR(sock, &masterSet);
                        } else{
                            buffer[bytes] = '\0';
                            std::cout << "Client " << sock << "sent: " << buffer << "\n";

                            send(sock, buffer, bytes, 0);
                        }
                    }
                }
            }
        }



};




#endif