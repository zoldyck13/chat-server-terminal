#ifndef __FTXUI_LOGIN
#define __FTXUI_LOGIN

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <functional>
#include <string>
#include "Dbclient.hpp"

using namespace ftxui;

class Login {

    private:
        Component layout;
        Component login_button, username_edit, password_edit, check_box, reg_link;
        
        std::string username, password;
        std::string login_msg = "";

        bool show_password = false;
        InputOption password_option;


    public:

        std::function<void()> onLog;
        std::function<void()> onRegister;


        Login(){

            password_option.password = true;
            check_box = Checkbox("show password", &show_password);

            username_edit = Input(&username, "Enter Username");
            password_edit = Input(&password, "Enter Password", password_option);


            InputOption pass_opt;
            pass_opt.password = show_password;      
            pass_opt.on_change = [&] {
                    return;
                };

            login_button = Button("Login", [&]{

                if(!checkLogin(db, username, password)){
                    login_msg = "Username or Password incorrect!";

                } else {

                    if(onLog) onLog();
                    login_msg = "";

                }
            });

            ButtonOption link_style;

            link_style.transform = [](const EntryState& state) {
                auto elem = text(state.label) | color(Color::Blue) | underlined;
                if (state.focused)
                    elem |= bold;
                return elem;
            };

             reg_link = Button("Register new account", [&]{
                if(onRegister) onRegister();
            }, link_style);



            auto container = Container::Vertical({
                username_edit,
                password_edit,
                check_box,
                login_button,
                reg_link,
            });

            layout = Renderer(container, [&]{

                //This method is dummy and not good at all, but i have tried many ways and only this work.
                //The problem with this method is render the Password input every frame.

                InputOption opt;
                opt.password = !show_password;

                password_edit = Input(&password, "Enter Password", opt);

                auto title = text(" LOGIN PANEL ") | bold | center;

                auto username_box = vbox({
                    text("Username") | color(Color::GrayLight),
                    username_edit->Render() | borderRounded,
                });

                auto password_box = vbox({
                    text("Password") | color(Color::GrayLight),
                    password_edit->Render() | borderRounded,
                });

                auto checkbox_row = hbox(check_box->Render());

                auto login_row =
                    login_button->Render() | center | borderRounded ;

                auto error_msg =
                    login_msg.empty()
                        ? filler()
                        : text(login_msg) | color(Color::RedLight) | center;

                auto register_row =
                    reg_link->Render() | center | borderRounded ;

                auto card = vbox({
                    title,
                    separator(),
                    username_box,
                    password_box,
                    checkbox_row,
                    separatorLight(),
                    login_row,
                    separator(),
                    register_row,
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

        Component RenderLogin() { return layout;};

};


#endif