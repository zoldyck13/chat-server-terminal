#ifndef __FTXUI_USER
#define __FTXUI_USER

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <vector>
#include "longin.hpp"
#include "utility.hpp"
#include "../Server.hpp"

using namespace ftxui;

class UserInfo : public Login  {

    private:

        Component layout;
        Component back_button;
        std::string username;
        std::string ip_dsiplay;

        std::vector<NetworkInterface> interface;

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

            ip_dsiplay = "Unkown";
            interface = GetLocalIPs();

            if(!interface.empty()) ip_dsiplay = interface[0].ip;




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
                            text("IP: " + ip_dsiplay ),
                            text("Port: " + std::to_string(Server::getInstace().getPort())),
                            hbox(text("Session: "), text(Server::getInstace().isRunning() ? "Active" : "Non-Active")),
                        }) | border | center | flex,
                    }) | border | center | flex, 
                        separator(),
                        back_button->Render(),
                }) | center | flex;
            });
        }


        Component UserRender(){return layout;}

};



#endif 