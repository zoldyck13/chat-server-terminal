#ifndef __LOGIN_FTXUI
#define __LOGIN_FTXUI

#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/util/ref.hpp>
#include "Dbserver.hpp"

using namespace ftxui;

class Login {
private:
    std::string user_name;
    std::string password;

    Component input_username;
    Component input_password;
    Component login_button;
    Component check_box;
    Component layout;
    Component reg_link;

    InputOption password_option;
    bool show_password = false;


public:


    std::function<void()> onRegister;

    //Create Style
    ButtonOption Style(){
        auto option = ButtonOption::Animated();
        option.transform = [] (const EntryState& s){
            auto element = text(s.label);
            if(s.focused){
                element |= bold;
            }
            return element | center | borderEmpty | flex;
        };
        return option;
    }

    Login() {

        // Create Inputs
        password_option.password = true;
        input_username = Input(&user_name, "Username");
        input_password = Input(&password, "Password", password_option);

        //Create checkbox
        check_box = Checkbox("show password ", &show_password);

        // Create Button once
        login_button = Button("Login", [&] {
            if (checkLogin(db, user_name, password))
                std::cout << "Enter successfully\n";
            else 
                std::cout << "failed\n";
        }, Style());

        // Create hyperlink 
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

        // Create container once
        auto container = Container::Vertical({
            input_username,
            input_password,
            login_button,
            check_box,
            reg_link
        });

        // Create renderer 
        layout = Renderer(container, [&] {

            InputOption opt;
            opt.password = !show_password;

            input_password = Input(&password, "Password", opt);

            return vbox({
                text("Admin Login UI"),
                separator(),
                hbox(text("Username: "), input_username->Render()) | border | flex,
                hbox(text("Password: "), input_password->Render()) | border | flex,
                separator(),
                login_button->Render(),
                separator(),
                check_box->Render(),
                separator(),
                reg_link->Render()

            }) | border | center | vcenter;
        });
    }

    

    Component RenderLogin() {
        return layout;
    }
};






#endif