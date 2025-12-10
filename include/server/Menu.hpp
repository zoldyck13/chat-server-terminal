#ifndef __FTUXI_MENU
#define __FTUXI_MENU

#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

using namespace ftxui;



class ServerMenu {
private:
    Component menu;
    Component layout;

    std::vector<std::string> entries = {
        "👤 User",
        "⚙️ Settings",
        "📁 Files",
        "🚀 Launch",
        "⎋ Logout"
    };

public:

    int selected = 0;

    MenuOption option;
      std::function<void(int)> onSelect;

    ServerMenu() {
        option = MenuOption();
        option.on_enter = [&] {
            if(onSelect)  onSelect(selected);
        };

        menu = Menu(&entries, &selected, option);


        layout = Renderer(menu, [&] {
            return vbox({
                hbox(text("📊 Main Menu: ") | bold | color(Color::Cyan) | center, text(std::to_string(selected))),
                separator(),
                menu->Render() | frame | flex,
            }) | size(WIDTH, GREATER_THAN, 20) | border | center | vcenter;
        });
    }

    Component RenderMenu() { return layout; }
};


#endif