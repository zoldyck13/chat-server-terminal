#ifndef __FTXUI_LAUNCH
#define __FTXUI_LAUNCH

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

#include <fstream>
#include <sstream>

#include <regex>
#include "../Server.hpp"

using namespace ftxui;


static std::string GetCPUUsageString() {
    // Very rough /proc/stat parsing (average, not perfect).
    static long long lastIdle = 0, lastTotal = 0;

    std::ifstream file("/proc/stat");
    std::string line;
    if (!std::getline(file, line))
        return "CPU: N/A";

    std::istringstream iss(line);
    std::string cpu;
    long long user, nice, system, idle, iowait, irq, softirq, steal;
    iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    long long idleAll = idle + iowait;
    long long nonIdle = user + nice + system + irq + softirq + steal;
    long long total = idleAll + nonIdle;

    long long totald = total - lastTotal;
    long long idled  = idleAll - lastIdle;

    lastTotal = total;
    lastIdle = idleAll;

    double cpu_percentage = 0.0;
    if (totald > 0)
        cpu_percentage = (double)(totald - idled) / totald * 100.0;

    std::ostringstream os;
    os << "CPU: " << (int)cpu_percentage << "%";
    return os.str();
}

static std::string GetRAMUsageString() {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) return "RAM: N/A";

    std::string key;
    long long value;
    std::string unit;
    long long memTotal = 0, memAvailable = 0;

    while (file >> key >> value >> unit) {
        if (key == "MemTotal:") memTotal = value;
        if (key == "MemAvailable:") {
            memAvailable = value;
            break;
        }
    }

    if (memTotal == 0) return "RAM: N/A";

    long long used = memTotal - memAvailable;

    std::ostringstream os;
    os << "RAM: " << used / 1024 << "MB / " << memTotal / 1024 << "MB";
    return os.str();
}



class LaunchServer {

private:
    Component layout;
    Component back_button;
    Component stop_button;
    Component restart_button;
    Component clear_logs_button;

    Component logs_checkbox;
    Component clients_checkbox;

    bool show_logs = true;
    bool show_clients = true;

public:

    LaunchServer() {
        back_button = Button("Back", [&] {
            if (onBack) onBack();
        });

        stop_button = Button("Stop Server", [&] {
            if (onStop) onStop();
        });

        restart_button = Button("Restart Server", [&] {
            if (onRestart) onRestart();
        });

        clear_logs_button = Button("Clear Logs", [&] {
            if (onClearLogs) onClearLogs();
        });

        logs_checkbox = Checkbox("Show Logs", &show_logs);
        clients_checkbox = Checkbox("Show Clients", &show_clients);

        auto container = Container::Vertical({
            back_button,
            stop_button,
            restart_button,
            clear_logs_button,
            logs_checkbox,
            clients_checkbox,
        });

        layout = Renderer(container, [&] {
            auto& server = Server::getInstace();

            // heartbeat
            static bool beat = false;
            beat = !beat;
            std::string heart = beat ? "♥" : "♡";

            // CPU/RAM
            std::string cpu_str = GetCPUUsageString();
            std::string ram_str = GetRAMUsageString();

            // Clients list (IP-based)
            std::vector<Element> client_list_elems;
            int clientCount = server.getClientCount();
            // If you later expose Server::getClients(), you can show IPs; for now just count
            if (clientCount == 0) {
                client_list_elems.push_back(text("No active clients"));
            } else {
                for (int i = 0; i < clientCount; ++i) {
                    client_list_elems.push_back(text("Client #" + std::to_string(i + 1)));
                }
            }

            // Logs with colors (INFO/WARN/ERROR/etc)
            std::vector<Element> log_items;
            for (auto& log : server.getLogs()) {
                auto e = text(log);
                std::string lower = log;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

                if (lower.find("error") != std::string::npos) {
                    e |= color(Color::Red) | bold;
                } else if (lower.find("disconnect") != std::string::npos) {
                    e |= color(Color::Yellow);
                } else if (lower.find("new client") != std::string::npos ||
                        lower.find("connected") != std::string::npos) {
                    e |= color(Color::Green);
                } else {
                    e |= color(Color::White);
                }
                log_items.push_back(e);
            }
            if (log_items.empty())
                log_items.push_back(text("No logs yet"));

            // Traffic stats
            uint64_t rx = server.getBytesReceived();
            uint64_t tx = server.getBytesSent();
            uint64_t msgs = server.getMessagesReceived();

            // Build panels conditionally (collapsible)
            Element clients_panel = text("");
            if (show_clients) {
                clients_panel = vbox({
                    text("Active Clients") | bold,
                    separator(),
                    vbox(client_list_elems),
                }) | border | flex;
            }

            Element logs_panel = text("");
            if (show_logs) {
                logs_panel = vbox({
                    text("Logs") | bold,
                    separator(),
                    vbox(log_items) | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 15),
                }) | border | flex;
            }

            // Layout
            return vbox({

                hbox({
                    text(" Server Dashboard ") | bold | center | flex,
                }) | border | color(Color::Blue),

                // STATUS, CPU, RAM, TRAFFIC
                vbox({
                    hbox({
                        text("Status: ") | bold,
                        text(server.isRunning() ? "RUNNING" : "STOPPED") |
                            color(server.isRunning() ? Color::Green : Color::Red) | bold,
                        text("   "),
                        text(heart) | color(Color::Red) | bold,
                        text("   IP: ") | bold,
                        text(server.getIP()),
                        text("   Port: ") | bold,
                        text(std::to_string(server.getPort())),
                        text("   Uptime: ") | bold,
                        text(server.getUptime()),
                    }) | border,

                    hbox({
                        text(cpu_str),
                        text("   "),
                        text(ram_str),
                        text("   RX: ") | bold,
                        text(std::to_string(rx) + " bytes"),
                        text("   TX: ") | bold,
                        text(std::to_string(tx) + " bytes"),
                        text("   Msgs: ") | bold,
                        text(std::to_string(msgs)),
                    }) | border,
                }),

                separator(),

                // CLIENTS + LOGS
                hbox({
                    clients_panel,
                    logs_panel,
                }),

                separator(),

                // Controls + toggles
                hbox({
                    back_button->Render() | border,
                    stop_button->Render() | border,
                    restart_button->Render() | border,
                    clear_logs_button->Render() | border,
                    text("  "),
                    logs_checkbox->Render(),
                    text("  "),
                    clients_checkbox->Render(),
                }) | center

            }) | center | vcenter;
        });
    }

    public:
        std::function<void()> onBack;
        std::function<void()> onStop;
        std::function<void()> onRestart;
        std::function<void()> onClearLogs;

        Component RenderLaunch() { return layout; }


};

#endif
