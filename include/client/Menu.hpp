#ifndef __FTXUI_MENU_USER
#define __FTXUI_MENU_USER

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <vector>


using namespace ftxui;


class ClientMenu {

    private:

    Component layout, menu;
    std::string username;

    std::vector<std::string> entries = {
        "💬  Enter Chat",
        "⚙️   Settings",
        "🖥️   Server Info",
        "🚪  Logout"
    };

    public:
    int select = 0;
    MenuOption option;
    std::function<void()> onSelect;

    ClientMenu() {

        option = MenuOption::Vertical();
        option.on_enter = [&] {
            if (onSelect) onSelect();
        };

        menu = Menu(&entries, &select, option);

        layout = Renderer(menu, [&] {

            auto title = vbox({
                text(" Welcome, " + username) | bold | center,
                text("Select an action to continue") 
                    | color(Color::GrayDark) | center,
            });

            auto menu_box =
                menu->Render()
                    | frame
                    | size(WIDTH, GREATER_THAN, 24)
                    | borderRounded;

            auto card = vbox({
                title,
                separatorLight(),
                menu_box,
            }) | borderDouble | color(Color::Cyan);

            return vbox({
                filler(),
                card | center,
                filler(),
            });
        });
    }

    Component RenderMenu() {return layout;}
    void setUsername(std::string name) { username = name;}


};


#endif 