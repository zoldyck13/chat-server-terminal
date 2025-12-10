#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>


class HandelClient; // forward declaration

class Server {

private:
    Server(int port);
    ~Server();  

    int serverSocket;
    std::string ip;
    int port;


    bool running = false;

    std::unique_ptr<HandelClient> hc;

    std::chrono::steady_clock::time_point start_time;

    std::thread server_thread;

public:

    static Server& getInstace(){
        static Server instace(8080);
        return instace;
    }

    Server(const Server&) = delete;

    Server& operator = (const Server&) = delete;

    void start();
    void stop();

    bool isRunning() const;
    std::string getIP() const { return ip; }
    int getPort() const { return port; }
    int getClientCount() const;
    std::vector<std::string> getLogs() const;
    std::string getUptime() const;

    void restart();               
    void clearLogs();             

    uint64_t getBytesReceived() const;
    uint64_t getBytesSent() const;
    uint64_t getMessagesReceived() const;

    void setPort(int p){ port = p;}

};

#endif
