#include "../include/client/ClientSocket.hpp"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>

ClientSocket::ClientSocket(int port)
    : port(port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
    }
}

bool ClientSocket::connectToServer() {
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("connect");
        return false;
    }

    connected = true;
    return true;
}

bool ClientSocket::login(const std::string& user, const std::string& password) {
    if (!connected) return false;

    std::string msg = "LOGIN " + user + " " + password;
    send(sockfd, msg.c_str(), msg.size(), 0);

    char buffer[256];
    int bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) return false;

    buffer[bytes] = '\0';
    std::string reply(buffer);

    if (reply.find("LOGIN OK") == 0) {
        logged_in = true;
        username = user;
        return true;
    }

    return false;
}


bool ClientSocket::sendMessage(const std::string& msg) {
    if (!connected || !logged_in) return false;

    std::string full = "MSG " + msg;
    send(sockfd, full.c_str(), full.size(), 0);
    return true;
}


bool ClientSocket::registerUser(const std::string& username,
                                const std::string& password)
{
    if (!connected)
        return false;

    std::string msg = "REGISTER " + username + " " + password + "\n";

    if (send(sockfd, msg.c_str(), msg.size(), 0) <= 0)
        return false;

    char buffer[256];
    int bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
        return false;

    buffer[bytes] = '\0';
    std::string response(buffer);

    return response.find("REGISTER OK") != std::string::npos;
}


void ClientSocket::startReceiver() {
    if (receiving) return;

    receiving = true;
    recv_thread = std::thread([this] {
        char buffer[512];
        std::string pending;   
        while (receiving) {
            int bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0)
                break;

            buffer[bytes] = '\0';
            pending += buffer;

            size_t pos;
            while ((pos = pending.find('\n')) != std::string::npos) {
                std::string line = pending.substr(0, pos);
                pending.erase(0, pos + 1);

                if (!line.empty())
                    chat_state.addMessage(line);

                if (onChatUpdate)
                    onChatUpdate();
            }
        }
    });
}





void ClientSocket::stopReceiver() {
    receiving = false;
    if (recv_thread.joinable())
        recv_thread.join();
}


void ClientSocket::sendChat(const std::string& msg) {
    if (!connected || !logged_in) return;
    std::string out = "MSG " + msg + "\n";
    send(sockfd, out.c_str(), out.size(), 0);
}



