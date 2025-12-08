#include <iostream>
#include "../include/server/longin.hpp"
#include "../include/server/Register.hpp"
#include "../include/server/Menu.hpp"


int main(){

    ScreenInteractive screen = ScreenInteractive::Fullscreen();
    InitializeDb();

    int page = 0;

    Login login_page;
    Register register_page;
    ServerMenu menu_page;

    Component container = Container::Tab({
        login_page.RenderLogin(),
        register_page.RenderRegister(),
        menu_page.RenderMenu()

    }, &page);

    login_page.onRegister = [&] {
        page = 1;
        screen.PostEvent(Event::Custom);
    };

    login_page.onLog = [&] {
        page = 2;
        screen.PostEvent(Event::Custom);
    };

    register_page.onBack = [&] {
        page = 0;
        screen.PostEvent(Event::Custom);
    };

    register_page.onReg = [&] {
       
       std::string user, pass;
        user = register_page.getUser();
        pass = register_page.getPass();

       if(insertUser(db, user, pass)) page = 0;

       screen.PostEvent(Event::Custom);

    };


    screen.Loop(container);
    return 0;
}