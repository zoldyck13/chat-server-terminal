#pragma once
#include <vector>
#include <string>
#include <mutex>

struct ChatState {
    std::vector<std::string> messages;
    std::mutex mtx;

    void addMessage(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx);
        messages.push_back(msg);
        if (messages.size() > 200) // limit
            messages.erase(messages.begin());
    }

    std::vector<std::string> getMessages() {
        std::lock_guard<std::mutex> lock(mtx);
        return messages;
    }
};
