#include "../include/server/longin.hpp"
#include "../include/server/Register.hpp"
#include "../include/server/Menu.hpp"
#include "../include/server/User.hpp"
#include "../include/server/Settings.hpp"
#include "../include/Server.hpp"
#include "../include/server/Launch.hpp"

 
int main(){ 

    ScreenInteractive screen = ScreenInteractive::Fullscreen();
    InitializeDb();

    int page = 0;

    Login login_page;
    Register register_page;
    ServerMenu menu_page;
    UserInfo user_page;
    Settingsinfo settings_page;
    LaunchServer launch_page;

    Component container = Container::Tab({
        login_page.RenderLogin(),
        register_page.RenderRegister(),
        menu_page.RenderMenu(),
        user_page.UserRender(),
        settings_page.RenderSettings(),
        launch_page.RenderLaunch(),
        settings_page.RenderIpSettings(),
        settings_page.RenderPortSettings(),
        settings_page.RenderTimeoutSettings(),
        settings_page.RenderChangeName(),
        settings_page.RenderChangePass(),

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

    //On select item from menu.
    menu_page.onSelect = [&](int index){
        if(index == 0) page = 3;
        else if(index == 1) page = 4;
        else if(index == 2){
             page = 5;

             Server::getInstace().start();
            
            }
        else if(index == 3) page = 0;

        screen.PostEvent(Event::Custom);
    };

    //On select item from settings
    settings_page.onSelectNetwork = [&](int index){
        if(index == 0) page = 6;
        else if(index == 1) page = 7;
        else if(index == 3) page = 8; 

        screen.PostEvent(Event::Custom);
    };

    settings_page.onSelectAccount = [&](int index) {
        if(index == 0) page = 9; 
        if(index == 1) page = 10;

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

    launch_page.onBack = [&] {
        page = 2;

        screen.PostEvent(Event::Custom);
    };

    launch_page.onStop = [&] {
        Server::getInstace().stop();
    };

    launch_page.onRestart = [&] {
        Server::getInstace().restart();
    };

    launch_page.onClearLogs = [&] {
        Server::getInstace().clearLogs();
        screen.PostEvent(Event::Custom);
    };

    settings_page.onBack2 = [&]{
        page = 4;
        screen.PostEvent(Event::Custom);
    };

    settings_page.onApplayChangeUsername = [&]{
        page = 9;
        std::string old_user = user_page.getUserName();
    
        settings_page.getOldUser(old_user);

        user_page.setUserName(settings_page.getNewUser());

        screen.PostEvent(Event::Custom);

    };

    settings_page.onApplayChangePass = [&] {

        std::string old_user = user_page.getUserName();

        settings_page.getOldUser(old_user);

    }; 



    std::thread refresher([&] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            screen.PostEvent(Event::Custom);
        }
    });
    refresher.detach();

    screen.Loop(container);


    return 0;
}