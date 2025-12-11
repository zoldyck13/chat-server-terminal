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
#include <functional>
#include <string>
#include "ftxui/dom/elements.hpp"
#include "utility.hpp"
#include "../Server.hpp"
#include "Dbserver.hpp"


using namespace ftxui;

class Settingsinfo {

    private:
        Component layout, layout_ip, layout_port, layout_changeusername, layout_changepass;
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

        int connection_timeout = 1000;       // default 1 second
        std::string timeout_input;
        std::string timeout_msg;

        Component timeout_edit;
        Component apply_timeout_btn;
        Component layout_timeout;

        Component apply_changeusername_btn;
        Component username_edit;
        std::string old_username;
        std::string new_username;
        std::string changeusername_msg;

        Component apply_changepass_btn;
        Component password_edit;
        Component oldpassword_edit;
        Component passwordconfi_edit;
        Component show_password_checkbox;
        bool show_password = false;
        std::string changepass_msg;
        std::string old_password;
        std::string confirm_password;
        std::string new_password;
        std::string confirm_message;



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
        std::function<void()> onApplayChangeUsername;
        std::function<void()> onApplayChangePass;

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


            // ================== TIMEOUT SETTINGS PAGE ==================
            timeout_input = std::to_string(connection_timeout);

            timeout_edit = Input(&timeout_input, "Timeout (ms)");

            apply_timeout_btn = Button("Apply Timeout", [this] {
                int new_timeout = 0;

                try {
                    new_timeout = std::stoi(timeout_input);
                } catch (...) {
                    timeout_msg = "Timeout must be a number!";
                    return;
                }

                if (new_timeout < 100 || new_timeout > 10000) {
                    timeout_msg = "Timeout must be between 100 and 10000 ms.";
                    return;
                }

                connection_timeout = new_timeout;
                timeout_msg = "Timeout updated successfully.";

                // You can save this to a future config system
                // Or apply to client later
            });


            auto container_timeout = Container::Vertical({
                timeout_edit,
                apply_timeout_btn,
                back_button_settings,
            });

            layout_timeout = Renderer(container_timeout, [this] {
                return vbox({
                    text("Connection Timeout Settings") | bold,
                    separator(),

                    hbox({
                        text("Current Timeout: ") | bold,
                        text(std::to_string(connection_timeout) + " ms")
                    }),

                    hbox({
                        text("New Timeout: ") | bold,
                        timeout_edit->Render()
                    }),

                    separator(),

                    hbox({
                        apply_timeout_btn->Render(),
                        text("   "),
                        back_button_settings->Render()
                    }),

                    separator(),
                    text(timeout_msg) | color(Color::Red),

                    separator(),
                    text("Recommended: 300–1000 ms") | dim

                }) | border | center | vcenter;
            });


            // ================== USERNAME SETTINGS PAGE ==================

            username_edit = Input(&new_username, "Enter new username");

            apply_changeusername_btn = Button("Apply", [&]{
                if(onApplayChangeUsername) onApplayChangeUsername();

                if(checkName(db, new_username)) {
                    // Username already in database
                    changeusername_msg = "User already exists, try another one.";
                } 
                else {
                    // Username available → update
                    if(updateUsername(db, old_username, new_username)) {
                        changeusername_msg = "Username " + new_username + " has changed successfully.";
                    }
                }
            });

            auto container_username = Container::Vertical({
                username_edit,
                apply_changeusername_btn,
                back_button_settings
            });

            layout_changeusername = Renderer(container_username, [&]{
                return vbox({
                    text("Change Username") | bold | center | flex,
                    separator(),
                    username_edit->Render() | border | center | flex,

                    hbox({
                        apply_changeusername_btn->Render(),
                        back_button_settings->Render(),
                    }) | flex,

                    separator(),
                    text(changeusername_msg) | dim,
                }) | border;
            }) | center | vcenter | flex;



            // ================== PASSWORD SETTINGS PAGE ==================

            int col = Color::Black;
            show_password_checkbox = Checkbox("show password ", &show_password);

            InputOption pass_opt;
            pass_opt.password = show_password;      
            pass_opt.on_change = [&] {
                if (confirm_password.empty()) {
                    confirm_message = "";
                    return;
                }

                if (new_password == confirm_password)
                    confirm_message = "✓ Passwords match";
                else
                    confirm_message = "✗ Passwords do NOT match";
            };

            oldpassword_edit = Input(&old_password, "Enter old password", pass_opt);
            password_edit = Input(&new_password, "Enter new password", pass_opt);
            passwordconfi_edit = Input(&confirm_password, "Confirm password", pass_opt);

            apply_changepass_btn = Button("Apply", [&]{
                if(onApplayChangePass) onApplayChangePass();

                if(!checkLogin(db, old_username, old_password)){
                    changepass_msg = "Old password is incorrect!";
                }
                else{
                   if(updatePassword(db, old_username, new_password)) {
                        changepass_msg = "Password has been changed successfully.";
                        col = Color::Green;
                    } else {
                        changepass_msg = "Failed to change password.";
                        col = Color::Black;
                    }
                }
            });

            auto container_pass = Container::Vertical({
            oldpassword_edit,
            password_edit,
            passwordconfi_edit,
            apply_changepass_btn,
            back_button_settings,
            show_password_checkbox
        });

            layout_changepass = Renderer(container_pass, [&] {
                return vbox({
                    text("Old Password:") | bold,
                    oldpassword_edit->Render(),

                    text("New Password:") | bold,
                    password_edit->Render(),

                    text("Confirm:") | bold,
                    passwordconfi_edit->Render(),

                    text(confirm_message) | color(confirm_message.find("✓") ? Color::Red : Color::Green),

                    separator(),

                    hbox(back_button_settings->Render(),apply_changepass_btn->Render()),
                    separator(),
                    text(changepass_msg) | dim | color(col == 2 ? Color::Green : Color::Black),

                    separator(),

                    show_password_checkbox->Render(),

                });
        }) | color(Color::Cyan);


        }

        Component RenderSettings()     { return layout; }
        Component RenderIpSettings()   { return layout_ip; }
        Component RenderPortSettings() { return layout_port; }
        Component RenderTimeoutSettings() {return layout_timeout;}
        Component RenderChangeName() {return layout_changeusername;}
        Component RenderChangePass() {return layout_changepass;}
        void getOldUser(std::string user){old_username = user; }
        std::string getNewUser() {return new_username;}
};





#endif