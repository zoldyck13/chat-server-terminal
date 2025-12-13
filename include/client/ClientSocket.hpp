#ifndef __FTXUI_CLIENTSOCKET
#define __FTXUI_CLIENTSOCKET

#include <string>
#include "ChatState.hpp"
#include <functional>
#include <thread>
#include <atomic>

#include "ChatState.hpp"


class ClientSocket {

    ChatState chat_state;
    std::function<void()> onChatUpdate;
    std::function<void(const std::string&)> onMessage;
    std::thread recv_thread;
    std::atomic<bool> receiving{false};

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
    bool registerUser(const std::string&, const std::string&);

    void startReceiver();
    void stopReceiver();
    void sendChat(const std::string& msg);
    ChatState& getChatState() { return chat_state; }

    void setOnChatUpdate(std::function<void()> cb) {
        onChatUpdate = std::move(cb);
    }


    void closeConnection();
};

#endif
