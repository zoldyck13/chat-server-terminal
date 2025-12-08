#include <iostream>
#include "../include/server/longin.hpp"
#include "../include/server/Register.hpp"

int main(){

    ScreenInteractive screen = ScreenInteractive::Fullscreen();
    InitializeDb();

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