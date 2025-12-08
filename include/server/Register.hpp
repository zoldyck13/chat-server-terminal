#ifndef __Register_FTXUI
#define __Register_FTXUI

#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/util/ref.hpp>

using namespace ftxui;

class Register {
private:
    std::string user_name;
    std::string password;
    std::string email;
    std::string conf_password;
    std::string unmatch_message;

    Component input_username;
    Component input_password;
    Component input_confirm_password;
    Component input_email;
    Component login_button;
    Component check_box;
    Component layout;
    Component sg_link;
    Component reg_button;

    InputOption password_option;
    bool show_password = false;

public:

    std::function<void()> onBack;
    std::function<void()> onReg;

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

    Register() {

        // Create Inputs
        password_option.password = true;
        input_username = Input(&user_name, "Username");
        input_email = Input(&email, "Email");
        input_password = Input(&password, "Password", password_option);
        input_confirm_password = Input(&conf_password, "Confirm Password", password_option);

        //Create checkbox
        check_box = Checkbox("show password ", &show_password);

        // Create Button to singin and for register 

        sg_link = Button("Sing in", [&]{
                if(onBack) onBack();
        }, ButtonOption::Simple());

        reg_button = Button("Register", [&]{
            if(password == conf_password){
                unmatch_message = "";
                if(onReg) onReg();
            } else unmatch_message = "Unmatch Password";
        }, Style());

        // Create container once
        auto container = Container::Vertical({
            input_username,
            input_email,
            input_password,
            input_confirm_password,
            sg_link,
            reg_button
        });

        layout = Renderer(container, [&] {

                return vbox({
                    text("This is the register page!"),
                    separator(),
                    hbox(text("* ") | color(Color::Red),text("Name: "), input_username->Render()) | border | flex,
                    hbox(text("* ") | color(Color::Red), text("Email: "), input_email->Render()) | border | flex,
                    hbox(text("* ") | color(Color::Red),text("Password: "), input_password->Render()) | border | flex,
                    hbox(text("* ") | color(Color::Red), text("Confirm password"), input_confirm_password->Render()) | border | flex,
                    separator(),
                    reg_button->Render(),
                    separator(),
                    sg_link->Render(),
                    text(unmatch_message) | color(Color::Red),

                }) | border | center | vcenter;
            });

}

    std::string getUser(){return user_name;}
    std::string getPass(){return password;}
    std::string getConPass(){return conf_password;}

    
    Component RenderRegister() {
        return layout;
    }
};






#endif