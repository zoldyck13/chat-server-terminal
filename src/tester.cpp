#include <iostream>
#include "../include/server/longin.hpp"
#include "../include/server/Register.hpp"
#include "../include/server/Menu.hpp"
#include "../include/server/User.hpp"

int main(){

    ScreenInteractive screen = ScreenInteractive::Fullscreen();
    InitializeDb();

    int page = 0;

    Login login_page;
    Register register_page;
    ServerMenu menu_page;
    UserInfo user_page;

    Component container = Container::Tab({
        login_page.RenderLogin(),
        register_page.RenderRegister(),
        menu_page.RenderMenu(),
        user_page.UserRender()

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

    menu_page.onSelect = [&](int index){
        if(index == 0) page = 3;
        if(index == 1) page = 4;
        if(index == 2) page = 5;
        if(index == 3) page = 6;

        screen.PostEvent(Event::Custom);
    };



    user_page.onBack = [&]{
        page = 2;

        screen.PostEvent(Event::Custom);
    };



    screen.Loop(container);
    return 0;
}