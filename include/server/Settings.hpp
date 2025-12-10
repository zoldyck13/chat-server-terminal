#ifndef __FTXUI_SETTINGS
#define __FTXUI_SETTINGS

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
#include "utility.hpp"
#include "../Server.hpp"


using namespace ftxui;

class Settingsinfo {

    private:
        Component layout, layout_ip, layout_port;
        Component back_button_menu, back_button_settings;
        Component account_settings_menu;
        Component network_settings_menu;

        // NEW: dedicated port settings components/state
        Component port_edit;
        Component apply_port_btn;
        Component back_button_port;

        std::string port_input;
        std::string dialog_msg;

        int selected_account = 0;
        int selected_network = 0;
        MenuOption option_account;
        MenuOption option_network;

        std::vector<std::string> account_settings_entries = {
            "Change username",
            "Change password",
            "Two-step verification (just display “coming soon”)",
            "Delete account (confirmation popup)",
        };

        std::vector<std::string> network_settings_entries = {
            "Server IP",
            "Server Port",
            "Auto-reconnect ON/OFF",
            "Connection timeout (ms)"
        };

    public:
        std::function<void()> onBack, onBack2;
        std::function<void(int)> onSelectAccount;
        std::function<void(int)> onSelectNetwork;

        Settingsinfo() {
            // ================== MAIN SETTINGS MENU ==================
            option_account = MenuOption();
            option_account.on_enter = [&] {
                if (onSelectAccount) onSelectAccount(selected_account);
            };

            option_network = MenuOption();
            option_network.on_enter = [&] {
                if (onSelectNetwork) onSelectNetwork(selected_network);
            };

            back_button_menu = Button("Back", [&] {
                if (onBack) onBack();
            });

            account_settings_menu = Menu(&account_settings_entries, &selected_account, option_account);
            network_settings_menu = Menu(&network_settings_entries, &selected_network, option_network);

            auto container = Container::Vertical({
                account_settings_menu,
                network_settings_menu,
                back_button_menu,
            });

            layout = Renderer(container, [&] {
                return vbox({
                    vbox({
                        text("🔑 Account Settings") | border | color(Color::Cyan),
                        separator(),
                        account_settings_menu->Render(),
                    }) | border | color(Color::Cyan),

                    separator(),

                    vbox({
                        text("🌐 Connection Settings") | border | color(Color::Cyan),
                        separator(),
                        network_settings_menu->Render(),
                    }) | border | color(Color::Cyan),

                    hbox(text(""), back_button_menu->Render()),
                }) | center | vcenter;
            });

            // ================== IP SETTINGS PAGE ==================
            back_button_settings = Button("Back", [&] {
                if (onBack2) onBack2();
            });

            auto container2 = Container::Vertical({
                back_button_settings,
            });

            layout_ip = Renderer(container2, [this] {
                std::vector<Element> ip_elems;
                auto interfaces = GetLocalIPs();

                for (auto& iface : interfaces) {
                    ip_elems.push_back(
                        hbox({
                            text(iface.name + ": ") | bold,
                            text(iface.ip),
                        })
                    );
                }

                return vbox({
                    text("Local Machine IPs") | bold,
                    separator(),
                    vbox(ip_elems) | border | color(Color::Cyan),
                    separator(),
                    hbox(back_button_settings->Render(), text("")),
                }) | center | vcenter | border;
            });

            // ================== PORT SETTINGS PAGE (FIXED) ==================
            port_input = std::to_string(Server::getInstace().getPort());

            port_edit = Input(&port_input, "Enter port");

            apply_port_btn = Button("Apply Port", [this] {
                int new_port = 0;
                try {
                    new_port = std::stoi(port_input);
                } catch (...) {
                    dialog_msg = "Port must be a number.";
                    return;
                }

                if (!IsValidPort(new_port)) {
                    dialog_msg = "Invalid port (1024–65535).";
                    return;
                }

                if (IsPortInUse(new_port)) {
                    dialog_msg = "Port is currently in use!";
                    return;
                }

                auto& srv = Server::getInstace();
                bool running = srv.isRunning();

                srv.setPort(new_port);

                if (running) {
                    srv.stop();
                    srv.start();
                    dialog_msg = "Port updated and server restarted.";
                } else {
                    dialog_msg = "Port updated.";
                }
            });

            back_button_port = Button("Back", [&] {
                if (onBack2) onBack2();
            });

            auto container3 = Container::Vertical({
                port_edit,
                apply_port_btn,
                back_button_port,
            });

            layout_port = Renderer(container3, [this] {
                return vbox({
                    text("Server Port Settings") | bold,
                    separator(),
                    hbox({
                        text("Current Port: ") | bold,
                        text(std::to_string(Server::getInstace().getPort())),
                    }),
                    hbox({
                        text("New Port: ") | bold,
                        port_edit->Render(),
                    }),
                    hbox({
                        apply_port_btn->Render(),
                        text("   "),
                        back_button_port->Render(),
                    }),
                    separator(),
                    text(dialog_msg) | color(Color::Red),
                    text("Recommended range: 1024 – 65535") | dim,
                }) | border | center | vcenter;
            });
        }

        Component RenderSettings()     { return layout; }
        Component RenderIpSettings()   { return layout_ip; }
        Component RenderPortSettings() { return layout_port; }
};





#endif