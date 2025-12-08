#ifndef __FTUXI_MENU
#define __FTUXI_EMNU

#include <string>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

using namespace ftxui;

MenuEntryOption Colored(ftxui::Color c) {
  MenuEntryOption option;
  option.transform = [c](EntryState state) {
    state.label = (state.active ? "> " : "  ") + state.label;
    Element e = text(state.label) | color(c);
    if (state.focused) {
      e = e | inverted;
    }
    if (state.active) {
      e = e | bold;
    }
    return e;
  };
  return option;
}



class ServerMenu {

    private:

    Component menu;
    Component layout;
    int selected = 0;

    public:

    ServerMenu(){
        menu = Container::Vertical({
            MenuEntry("1. Connected", Colored(Color::Cyan)),
            MenuEntry("2. Settings", Colored(Color::Cyan)),
            MenuEntry("3. Admins List", Colored(Color::Cyan)),
        }, &selected);


        layout = Renderer(menu, [&]{
            return vbox({
                hbox(text("selected = "), text(std::to_string(selected))),
                separator(),
                menu->Render() | frame | size(HEIGHT, LESS_THAN, 100),
            }) | border | center | vcenter;
        });
    }

    Component RenderMenu(){return layout;}

};



#endif