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
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

#include "../Server.hpp"

using namespace ftxui;

// ============================================================
// SYSTEM METRICS (NO UI LOGIC HERE)
// ============================================================

static std::string ReadCPU() {
    static long long lastIdle = 0, lastTotal = 0;

    std::ifstream file("/proc/stat");
    std::string line;
    if (!std::getline(file, line)) return "CPU: N/A";

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

    int percent = 0;
    if (totald > 0)
        percent = (int)((double)(totald - idled) / totald * 100.0);

    return "CPU: " + std::to_string(percent) + "%";
}

static std::string ReadRAM() {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) return "RAM: N/A";

    std::string key, unit;
    long long value;
    long long total = 0, avail = 0;

    while (file >> key >> value >> unit) {
        if (key == "MemTotal:") total = value;
        if (key == "MemAvailable:") {
            avail = value;
            break;
        }
    }

    long long used = total - avail;
    return "RAM: " + std::to_string(used / 1024) + "MB / " +
           std::to_string(total / 1024) + "MB";
}

// ============================================================
// DASHBOARD STATE (THREAD-SAFE CACHE)
// ============================================================

struct DashboardState {
    std::string cpu = "CPU: ?";
    std::string ram = "RAM: ?";
    std::string uptime = "Uptime: ?";
    std::mutex mtx;
};

// ============================================================
// LAUNCH SERVER UI
// ============================================================

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

    DashboardState state;

public:
    LaunchServer() {

        back_button = Button("Back", [&] { if (onBack) onBack(); });
        stop_button = Button("Stop Server", [&] { if (onStop) onStop(); });
        restart_button = Button("Restart Server", [&] { if (onRestart) onRestart(); });
        clear_logs_button = Button("Clear Logs", [&] { if (onClearLogs) onClearLogs(); });

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

        // ==============================
        // BACKGROUND MONITOR THREAD
        // ==============================
        std::thread([this] {
            while (true) {
                {
                    std::lock_guard<std::mutex> lock(state.mtx);
                    state.cpu = ReadCPU();
                    state.ram = ReadRAM();
                    state.uptime = Server::getInstace().getUptime();
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }).detach();

        // ==============================
        // UI RENDERER (PURE RENDER)
        // ==============================
        layout = Renderer(container, [&] {

            auto& server = Server::getInstace();

            static bool beat = false;
            beat = !beat;
            std::string heart = beat ? "♥" : "♡";

            // -------- COPY STATE SAFELY --------
            std::string cpu, ram, uptime;
            {
                std::lock_guard<std::mutex> lock(state.mtx);
                cpu = state.cpu;
                ram = state.ram;
                uptime = state.uptime;
            }

            // -------- CLIENT LIST --------
            std::vector<Element> clients;
            int count = server.getClientCount();
            if (count == 0)
                clients.push_back(text("No active clients"));
            else
                for (int i = 0; i < count; ++i)
                    clients.push_back(text("Client #" + std::to_string(i + 1)));

            // -------- LOGS (LIMITED) --------
            std::vector<Element> log_items;
            auto logs = server.getLogs();
            int start = logs.size() > 100 ? logs.size() - 100 : 0;
            for (size_t i = start; i < logs.size(); ++i)
                log_items.push_back(paragraph(logs[i]));

            if (log_items.empty())
                log_items.push_back(text("No logs"));

            // -------- PANELS --------
            Element clients_panel = text("");
            if (show_clients) {
                clients_panel = vbox({
                    text("Clients") | bold,
                    separator(),
                    vbox(clients),
                }) | border | flex;
            }

            Element logs_panel = text("");
            if (show_logs) {
                logs_panel = vbox({
                    text("Logs") | bold,
                    separator(),
                    vbox(log_items)
                        | frame
                        | vscroll_indicator
                        | size(HEIGHT, EQUAL, 15)
                }) | border | flex;
            }


            return vbox({

                text(" Server Dashboard ") | bold | center | border,

                hbox({
                    text("Status: "),
                    text(server.isRunning() ? "RUNNING" : "STOPPED")
                        | color(server.isRunning() ? Color::Green : Color::Red),
                    text("   "),
                    text(heart) | color(Color::Red),
                    text("   Uptime: "),
                    text(uptime),
                }) | border,

                hbox({
                    text(cpu),
                    text("   "),
                    text(ram),
                    text("   RX: "), text(std::to_string(server.getBytesReceived())),
                    text("   TX: "), text(std::to_string(server.getBytesSent())),
                    text("   Msgs: "), text(std::to_string(server.getMessagesReceived())),
                }) | border,

                separator(),

                hbox({
                    clients_panel,
                    logs_panel,
                }),

                separator(),

                hbox({
                    back_button->Render(),
                    stop_button->Render(),
                    restart_button->Render(),
                    clear_logs_button->Render(),
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
