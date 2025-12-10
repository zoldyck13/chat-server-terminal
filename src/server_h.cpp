#include "../include/Server.hpp"
#include "../include/handelclient.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
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

    hc->run();
}

void Server::stop() {
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
