#ifndef __FTXUI_REGISTER_USER
#define __FTXUI_REGISTER_USER

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/dom/elements.hpp>

#include <string>
#include <functional>

#include "../client/ClientSocket.hpp"

using namespace ftxui;

class Register {

private:
    Component layout;
    Component username_edit, password_edit, confirm_password_edit, checkbox;
    Component register_btn, back_btn;

    std::string username, password, confirm_password;
    std::string errMsg, confirm_message;

    bool show_password = false;
    InputOption password_option;

public:
    std::function<void()> onBack;
    std::function<void()> onRegister;

    Register() {

        password_option.password = false;

        checkbox = Checkbox("show password", &show_password);

        username_edit = Input(&username, "Enter Username");
        password_edit = Input(&password, "Enter Password", password_option);
        confirm_password_edit = Input(&confirm_password, "Confirm Password", password_option);

        register_btn = Button("Register", [&]{
            if (password != confirm_password) {
                errMsg = "Unmatch Password";
                return;
            }

            auto& client = ClientSocket::getInstace();

            if (!client.registerUser(username, password)) {
                errMsg = "Server rejected register";
                return;
            }

            errMsg = "";
            if (onBack) onBack(); 
        });


        back_btn = Button("Back", [&] {
            if (onBack) onBack();
        });

        auto container = Container::Vertical({
            username_edit,
            password_edit,
            confirm_password_edit,
            checkbox,
            register_btn,
            back_btn,
        });

        layout = Renderer(container, [&] {

            InputOption opt;
            opt.password = !show_password;
            opt.on_change = [&] {
                if (confirm_password.empty()) {
                    confirm_message.clear();
                    return;
                }

                confirm_message =
                    (password == confirm_password)
                        ? "✓ Passwords match"
                        : "✗ Passwords do NOT match";
            };

            password_edit = Input(&password, "Enter Password", opt);
            confirm_password_edit = Input(&confirm_password, "Confirm Password", opt);

            auto title = text(" REGISTER PANEL ") | bold | center;

            auto username_box = vbox({
                text("Username") | color(Color::GrayLight),
                username_edit->Render() | borderRounded,
            });

            auto password_box = vbox({
                text("Password") | color(Color::GrayLight),
                password_edit->Render() | borderRounded,
            });

            auto password_conf_box = vbox({
                text("Rewrite Password") | color(Color::GrayLight),
                confirm_password_edit->Render() | borderRounded,
            });

            auto conf_msg =
                confirm_message.empty()
                    ? filler()
                    : text(confirm_message)
                        | ((password == confirm_password)
                            ? color(Color::GreenLight)
                            : color(Color::RedLight))
                        | center | dim;

            auto error_msg =
                errMsg.empty()
                    ? filler()
                    : text(errMsg) | color(Color::RedLight) | center | dim;

            auto card = vbox({
                title,
                separator(),
                username_box,
                password_box,
                password_conf_box,
                checkbox->Render(),
                conf_msg,
                separatorLight(),
                register_btn->Render() | center | borderRounded,
                separator(),
                back_btn->Render() | center | borderRounded,
                separator(),
                error_msg,
            }) | borderDouble | size(WIDTH, EQUAL, 50) | color(Color::Cyan);

            return vbox({
                filler(),
                card | center,
                filler()
            });
        });
    }

    Component RenderRegister() { return layout; }
};

#endif
