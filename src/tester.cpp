#include "../include/server/longin.hpp"
#include "../include/server/Register.hpp"
#include "../include/server/Menu.hpp"
#include "../include/server/User.hpp"
#include "../include/server/Settings.hpp"


int main(){

    ScreenInteractive screen = ScreenInteractive::Fullscreen();
    InitializeDb();

    int page = 0;

    Login login_page;
    Register register_page;
    ServerMenu menu_page;
    UserInfo user_page;
    Settingsinfo settings_page;

    Component container = Container::Tab({
        login_page.RenderLogin(),
        register_page.RenderRegister(),
        menu_page.RenderMenu(),
        user_page.UserRender(),
        settings_page.RenderSettings()

    }, &page);

    login_page.onRegister = [&] {
        page = 1;
        screen.PostEvent(Event::Custom);
    };

    login_page.onLog = [&] {
        user_page.setUserName(login_page.getUserName());
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
        else if(index == 1) page = 4;
        else if(index == 2) page = 5;
        else if(index == 3) page = 0;

        screen.PostEvent(Event::Custom);
    };



    user_page.onBack = [&]{
        page = 2;

        screen.PostEvent(Event::Custom);
    };

    settings_page.onBack = [&] {
        page = 2;

        screen.PostEvent(Event::Custom);
    };



    screen.Loop(container);
    return 0;
}