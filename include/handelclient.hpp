#ifndef HANDEL_CLIENT_HPP
#define HANDEL_CLIENT_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


class HandelClient {
private:
    int serverSocket;
    fd_set masterSet, readSet;
    int maxFD;

    bool running = false;

    std::vector<int> clients;
    std::vector<std::string> logs;

    void addLog(const std::string& msg) {
        logs.push_back(msg);
        std::cout << msg << "\n";
    }

public:
    HandelClient(int serverSocket)
        : serverSocket(serverSocket)
    {
        FD_ZERO(&masterSet);
        FD_ZERO(&readSet);
        FD_SET(serverSocket, &masterSet);

        maxFD = serverSocket;
    }

    bool isRunning() const { return running; }
    int getClientCount() const { return clients.size(); }
    std::vector<std::string> getLogs() const { return logs; }

    void stop() { running = false; }

    void run() {
        running = true;
        addLog("Server loop started.");

        while (running) {
            readSet = masterSet;

            int activity = select(maxFD + 1, &readSet, nullptr, nullptr, nullptr);
            if (activity < 0) continue;

            if (FD_ISSET(serverSocket, &readSet)) {
                int clientSocket = accept(serverSocket, nullptr, nullptr);
                clients.push_back(clientSocket);
                addLog("New client connected: " + std::to_string(clientSocket));

                FD_SET(clientSocket, &masterSet);
                if (clientSocket > maxFD) maxFD = clientSocket;
            }

            for (int sock = 0; sock <= maxFD; sock++) {
                if (sock != serverSocket && FD_ISSET(sock, &readSet)) {
                    char buffer[1024];
                    int bytes = recv(sock, buffer, sizeof(buffer), 0);

                    if (bytes <= 0) {
                        addLog("Client disconnected: " + std::to_string(sock));
                        close(sock);
                        FD_CLR(sock, &masterSet);

                        clients.erase(
                            std::remove(clients.begin(), clients.end(), sock),
                            clients.end()
                        );
                    }
                    else {
                        buffer[bytes] = '\0';
                        addLog("Client " + std::to_string(sock) + " sent: " + buffer);
                        send(sock, buffer, bytes, 0);
                    }
                }
            }
        }
        addLog("Server loop stopped.");
    }
};

#endif
