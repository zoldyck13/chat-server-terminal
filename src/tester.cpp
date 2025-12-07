#include <iostream>
#include "../include/server/longin.hpp"
#include "../include/server/Register.hpp"

int main(){

    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    int page = 0;

    Login login_page;
    Register register_page;

    Component container = Container::Tab({
        login_page.RenderLogin(),
        register_page.RenderRegister()
    }, &page);

    login_page.onRegister = [&] {
        page = 1;
        screen.PostEvent(Event::Custom);
    };

    register_page.onBack = [&] {
        page = 0;
        screen.PostEvent(Event::Custom);
    };

    screen.Loop(container);
    return 0;
}