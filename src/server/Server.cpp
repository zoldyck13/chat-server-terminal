#include "../../include/Server.hpp"
#include "../../include/handelclient.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>


Server::Server(int port)
    : port(port), running(false)
{
    ip = "0.0.0.0";
}

Server::~Server() {}

void Server::start() {
    running = true;
    start_time = std::chrono::steady_clock::now();

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&addr, sizeof(addr));
    listen(serverSocket, 5);

    hc = std::make_unique<HandelClient>(serverSocket);

    server_thread = std::thread([this]{
        hc->run();
    });

    server_thread.detach();
}

void Server::stop() {
    if (!running) return;

    running = false;

    if (hc)
        hc->stop();

    close(serverSocket);
}

bool Server::isRunning() const {
    return running && hc && hc->isRunning();
}



int Server::getClientCount() const {
    return hc ? hc->getClientCount() : 0;
}

std::vector<std::string> Server::getLogs() const {   
    return hc ? hc->getLogs() : std::vector<std::string>{};
}

std::string Server::getUptime() const {
    if (!running) return "0s";

    auto now = std::chrono::steady_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();

    return std::to_string(seconds) + "s";
}


void Server::restart() {
    stop();
    start();
}

void Server::clearLogs() {
    if (hc) hc->clearLogs();
}

uint64_t Server::getBytesReceived() const {
    return hc ? hc->getBytesReceived() : 0;
}

uint64_t Server::getBytesSent() const {
    return hc ? hc->getBytesSent() : 0;
}

uint64_t Server::getMessagesReceived() const {
    return hc ? hc->getMessagesReceived() : 0;
}

bool Server::getClientActive() const {
    return hc && hc->getClientCount() > 0;
}
