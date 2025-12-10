#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <memory>
#include <chrono>

class HandelClient; // forward declaration

class Server {
private:
    int serverSocket;
    std::string ip;
    int port;

    bool running = false;

    std::unique_ptr<HandelClient> hc;

    std::chrono::steady_clock::time_point start_time;

public:
    Server(int port);
    ~Server();  


    void start();
    void stop();

    bool isRunning() const;
    std::string getIP() const { return ip; }
    int getPort() const { return port; }
    int getClientCount() const;
    std::vector<std::string> getLogs() const;
    std::string getUptime() const;
};

#endif
