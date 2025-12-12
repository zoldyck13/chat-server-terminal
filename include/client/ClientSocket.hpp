#ifndef __FTXUI_CLIENTSOCKET
#define __FTXUI_CLIENTSOCKET

#include <string>

class ClientSocket {
private:
    int sockfd;
    int port;
    std::string username;
    bool connected = false;
    bool logged_in = false;

    ClientSocket(int port);

public:

    std::string getUsername() const {
        return logged_in ? username : "";
    }

    static ClientSocket& getInstace() {
        static ClientSocket instance(8080);
        return instance;
    }

    ClientSocket(const ClientSocket&) = delete;
    ClientSocket& operator=(const ClientSocket&) = delete;

    bool connectToServer();
    bool login(const std::string& username, const std::string& password);
    bool sendMessage(const std::string& msg);
    std::string receiveMessage();

    void closeConnection();
};

#endif
