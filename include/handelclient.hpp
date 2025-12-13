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

#include "../include/server/Dbserver.hpp"
#include <sstream>






class HandelClient {
private:

    std::atomic<uint64_t> sid_generator{1};

    struct ClientInfo {
    int fd;
    uint64_t sid;        
    int user_id;     
    std::string username;
    bool authenticated;
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


    //============================
    //FIND CLIENT
    //============================

    ClientInfo* find_client_by_fd(int fd) {
        for (auto& c : clients)
            if (c.fd == fd)
                return &c;
        return nullptr;
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


                    ClientInfo client;
                    client.fd = clientSocket;
                    client.sid = sid_generator++;
                    client.user_id = -1;
                    client.username = "";
                    client.authenticated = false;

                    {
                        std::lock_guard<std::mutex> lock(clients_mutex);
                        clients.push_back(client);
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

                        ClientInfo* c = find_client_by_fd(sock);
                        if (!c) continue;

                        std::string msg(buffer);

                        // ================= REGISTER =================
                        if (msg.rfind("REGISTER ", 0) == 0) {

                            std::istringstream iss(msg);
                            std::string cmd, username, password;
                            iss >> cmd >> username >> password;

                            if (insertUser(db, username, password)) {
                                send(sock, "REGISTER OK\n", 12, 0);
                                addLog("New user registered: " + username);
                            } else {
                                send(sock, "REGISTER FAILED\n", 16, 0);
                            }
                            continue;
                        }

                        // ================= LOGIN =================
                        if (!c->authenticated && msg.rfind("LOGIN ", 0) == 0) {

                            std::istringstream iss(msg);
                            std::string cmd, username, password;
                            iss >> cmd >> username >> password;

                            if (checkLogin(db, username, password)) {
                                c->username = username;
                                c->authenticated = true;

                                addLog("SID " + std::to_string(c->sid) +
                                    " logged in as " + username);

                                send(sock, "LOGIN OK\n", 9, 0);
                            } else {
                                send(sock, "LOGIN FAILED\n", 13, 0);
                            }
                            continue;
                        }

                        // ================= BLOCK UNAUTH =================
                        if (!c->authenticated) {
                            send(sock, "Please login first\n", 19, 0);
                            continue;
                        }

                        // ================= CHAT =================
                        if (msg.rfind("MSG ", 0) == 0) {

                            std::string text = msg.substr(4);
                            if (text.empty()) continue;

                            std::string full_msg = c->username + ": " + text + "\n";
                            addLog(full_msg);

                            std::lock_guard<std::mutex> lock(clients_mutex);
                            for (auto& client : clients) {
                                send(client.fd, full_msg.c_str(), full_msg.size(), 0);
                            }
                        }

                    }
                }
            }
        }

        addLog("Server loop stopped.");
    }

    bool getClientActive() const {
    return !clients.empty();
    }

};

#endif
