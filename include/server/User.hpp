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

class UserInfo : protected Login  {

    private:

        Component layout;
        Component back_button;

    public:

        std::function<void()> onBack;

        UserInfo(){

            back_button = Button("Back", [&]{
                if(onBack) onBack();
            }, ButtonOption::Simple());

            auto container = Container::Vertical({
                back_button
            });

            layout = Renderer(container, [&] {
                return vbox ({
                    text("Name: " + user_name)| border | flex,
                    separator(),
                    back_button->Render() | flex | border,
                }) | border | center;

            });
        }


        Component UserRender(){return layout;}

};



#endif 