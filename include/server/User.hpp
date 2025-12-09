#ifndef __FTXUI_USER
#define __FTXUI_USER

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include "longin.hpp"


using namespace ftxui;

class UserInfo : public Login  {

    private:

        Component layout;
        Component back_button;
        std::string username;

    public:

        std::function<void()> onBack;

        void setUserName(const std::string& name){ username = name;}

        UserInfo(){

            back_button = Button("Back", [&]{
                if(onBack) onBack();
            }, ButtonOption::Simple());

            auto container = Container::Vertical({
                back_button
            });

            layout = Renderer(container, [&] {
                return vbox({
                    hbox({
                        text(" Account Info") | bold | center | flex,
                    }),

                    separator(),

                    vbox({
                        text("Name: " + username),
                        text("Role: Admin"),
                        text("Status: Online"),

                        vbox({
                            text("Connection Info") | bold,
                            separator(),
                            text("IP: 192.168.0.3"),
                            text("Port: 5500"),
                            text("Session: Active"),
                        }) | border,
                    }) | border, 
                        separator(),
                        hbox({
                            back_button->Render() | border,
                            text("")
                        })
                }) | center;
            });
        }


        Component UserRender(){return layout;}

};



#endif 