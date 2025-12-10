#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>
#include <string>
#include <unistd.h>


struct NetworkInterface {
    std::string name;
    std::string ip;
};

std::vector<NetworkInterface> GetLocalIPs() {
    std::vector<NetworkInterface> result;

    struct ifaddrs* interfaces = nullptr;
    struct ifaddrs* temp = nullptr;

    if (getifaddrs(&interfaces) == -1) {
        return result;  // return empty on failure
    }

    temp = interfaces;
    while (temp != nullptr) {
        if (temp->ifa_addr && temp->ifa_addr->sa_family == AF_INET) {

            void* addrPtr = &((struct sockaddr_in*)temp->ifa_addr)->sin_addr;

            char ipBuf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, addrPtr, ipBuf, INET_ADDRSTRLEN);

            result.push_back({
                temp->ifa_name, 
                ipBuf
            });
        }
        temp = temp->ifa_next;
    }

    freeifaddrs(interfaces);
    return result;
}

bool IsPortInUse(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int result = bind(sock, (sockaddr*)&addr, sizeof(addr));
    close(sock);

    return result < 0;  // if bind fails → port is in use
}

bool IsValidPort(int port) {
    return port >= 1024 && port <= 65535;
}
