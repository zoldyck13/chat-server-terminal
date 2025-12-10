#ifndef HANDEL_CLIENT_HPP
#define HANDEL_CLIENT_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class HandelClient {
private:
    struct ClientInfo {
        int fd;
        std::string ip;
    };

    int serverSocket;
    fd_set masterSet, readSet;
    int maxFD;

    bool running = false;

    std::vector<ClientInfo> clients;
    std::vector<std::string> logs;

    // thread safety
    mutable std::mutex clients_mutex;
    mutable std::mutex logs_mutex;

    // traffic stats
    std::atomic<uint64_t> bytes_received{0};
    std::atomic<uint64_t> bytes_sent{0};
    std::atomic<uint64_t> messages_received{0};

    void addLog(const std::string& msg) {
        std::lock_guard<std::mutex> lock(logs_mutex);
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

    // =============================
    // Getters for Dashboard
    // =============================

    bool isRunning() const { return running; }

    int getClientCount() const {
        std::lock_guard<std::mutex> lock(clients_mutex);
        return clients.size();
    }

    std::vector<std::string> getLogs() const {
        std::lock_guard<std::mutex> lock(logs_mutex);
        return logs;
    }

    std::vector<ClientInfo> getClients() const {
        std::lock_guard<std::mutex> lock(clients_mutex);
        return clients;
    }

    uint64_t getBytesReceived() const { return bytes_received.load(); }
    uint64_t getBytesSent() const { return bytes_sent.load(); }
    uint64_t getMessagesReceived() const { return messages_received.load(); }

    void clearLogs() {
        std::lock_guard<std::mutex> lock(logs_mutex);
        logs.clear();
    }

    // =============================
    // Control
    // =============================

    void stop() { running = false; }

    void run() {
        running = true;
        addLog("Server loop started.");

        while (running) {

            readSet = masterSet;
            int activity = select(maxFD + 1, &readSet, nullptr, nullptr, nullptr);
            if (activity < 0) continue;

            // ============================
            // ACCEPT NEW CLIENT
            // ============================
            if (FD_ISSET(serverSocket, &readSet)) {
                sockaddr_in client_addr{};
                socklen_t addr_len = sizeof(client_addr);

                int clientSocket = accept(serverSocket, (sockaddr*)&client_addr, &addr_len);

                if (clientSocket >= 0) {

                    char ip_buf[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(client_addr.sin_addr), ip_buf, INET_ADDRSTRLEN);

                    {
                        std::lock_guard<std::mutex> lock(clients_mutex);
                        clients.push_back({ clientSocket, ip_buf });
                    }

                    addLog(std::string("New client connected: fd=") + std::to_string(clientSocket) +
                           " ip=" + ip_buf);

                    FD_SET(clientSocket, &masterSet);
                    if (clientSocket > maxFD)
                        maxFD = clientSocket;
                }
            }

            // ============================
            // HANDLE EXISTING CLIENTS
            // ============================
            for (int sock = 0; sock <= maxFD; sock++) {

                if (sock != serverSocket && FD_ISSET(sock, &readSet)) {
                    char buffer[1024];
                    int bytes = recv(sock, buffer, sizeof(buffer), 0);

                    // Client disconnected
                    if (bytes <= 0) {
                        addLog("Client disconnected: fd=" + std::to_string(sock));
                        close(sock);
                        FD_CLR(sock, &masterSet);

                        std::lock_guard<std::mutex> lock(clients_mutex);
                        clients.erase(std::remove_if(clients.begin(), clients.end(),
                                        [&](const ClientInfo& c){ return c.fd == sock; }),
                                      clients.end());
                    }
                    else {
                        buffer[bytes] = '\0';

                        bytes_received += bytes;
                        messages_received++;

                        addLog("Client " + std::to_string(sock) + " sent: " + buffer);

                        int sent = send(sock, buffer, bytes, 0);
                        if (sent > 0)
                            bytes_sent += sent;
                    }
                }
            }
        }

        addLog("Server loop stopped.");
    }
};

#endif
