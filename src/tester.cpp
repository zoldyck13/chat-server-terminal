#include "../include/server/longin.hpp"
#include "../include/server/Register.hpp"
#include "../include/server/Menu.hpp"
#include "../include/server/User.hpp"
#include "../include/server/Settings.hpp"
#include "../include/Server.hpp"
#include "../include/server/Launch.hpp"

 
int main() {

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
        login_page.RenderLogin(),          // 0
        register_page.RenderRegister(),    // 1
        menu_page.RenderMenu(),            // 2
        user_page.UserRender(),            // 3
        settings_page.RenderSettings(),    // 4
        launch_page.RenderLaunch(),        // 5
        settings_page.RenderIpSettings(),  // 6
        settings_page.RenderPortSettings(),// 7
        settings_page.RenderTimeoutSettings(), // 8
        settings_page.RenderChangeName(),  // 9
        settings_page.RenderChangePass(),  // 10
    }, &page);

    // ================= LOGIN =================
    login_page.onRegister = [&] {
        page = 1;
        screen.PostEvent(Event::Custom);
    };

    login_page.onLog = [&] {
        user_page.setUserName(login_page.getUserName());
        page = 2;
        screen.PostEvent(Event::Custom);
    };

    // ================= REGISTER =================
    register_page.onBack = [&] {
        page = 0;
        screen.PostEvent(Event::Custom);
    };

    register_page.onReg = [&] {
        const auto& user = register_page.getUser();
        const auto& pass = register_page.getPass();

        if (insertUser(db, user, pass)) {
            page = 0;
        }
        screen.PostEvent(Event::Custom);
    };

    // ================= MENU =================
    menu_page.onSelect = [&](int index) {
        switch (index) {
            case 0: page = 3; break; // User info
            case 1: page = 4; break; // Settings
            case 2:
                page = 5; // Launch
                Server::getInstace().start();
                break;
            case 3:
                page = 0; // Logout
                break;
        }
        screen.PostEvent(Event::Custom);
    };

    // ================= SETTINGS =================
    settings_page.onSelectNetwork = [&](int index) {
        if (index == 0) page = 6;
        else if (index == 1) page = 7;
        else if (index == 3) page = 8;
        screen.PostEvent(Event::Custom);
    };

    settings_page.onSelectAccount = [&](int index) {
        if (index == 0) page = 9;
        else if (index == 1) page = 10;
        screen.PostEvent(Event::Custom);
    };

    settings_page.onBack = [&] {
        page = 2;
        screen.PostEvent(Event::Custom);
    };

    settings_page.onBack2 = [&] {
        page = 4;
        screen.PostEvent(Event::Custom);
    };

    // ================= USER =================
    user_page.onBack = [&] {
        page = 2;
        screen.PostEvent(Event::Custom);
    };

    // ================= LAUNCH =================
    launch_page.onBack = [&] {
        page = 2;
        screen.PostEvent(Event::Custom);
    };

    launch_page.onStop = [&] {
        Server::getInstace().stop();
        screen.PostEvent(Event::Custom);
    };

    launch_page.onRestart = [&] {
        Server::getInstace().restart();
        screen.PostEvent(Event::Custom);
    };

    launch_page.onClearLogs = [&] {
        Server::getInstace().clearLogs();
        screen.PostEvent(Event::Custom);
    };

    screen.Loop(container);
    return 0;
}
