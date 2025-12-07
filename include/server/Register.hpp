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

    Component input_username;
    Component input_password;
    Component login_button;
    Component check_box;
    Component layout;
    Component sg_link;

    InputOption password_option;
    bool show_password = false;

public:

    std::function<void()> onBack;

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
        input_password = Input(&password, "Password", password_option);

        //Create checkbox
        check_box = Checkbox("show password ", &show_password);

        // Create Button to singin

        sg_link = Button("Sing in", [&]{
            if(onBack) onBack();
        }, ButtonOption::Simple());

        // Create container once
        auto container = Container::Vertical({
            input_username,
            input_password,
            sg_link
        });

        layout = Renderer(container, [&] {
            InputOption opt;
            opt.password = !show_password;

            input_password = Input(&password, "Password", opt);

                return vbox({
                    text("This is the register page!"),
                    separator(),
                    hbox(text("Username: "), input_username->Render()),
                    hbox(text("Password: "), input_password->Render()),
                    separator(),
                    sg_link->Render()

                }) | border | center | vcenter;
            });

}

    

    
    Component RenderRegister() {
        return layout;
    }
};






#endif