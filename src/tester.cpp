#include <iostream>
#include "../include/server/longin.hpp"


int main(){

    Login* login = new Login();

    ftxui::Component render_login = login->RenderLogin();

    auto screen = ScreenInteractive::TerminalOutput();
    screen.Loop(render_login);

    delete login;
    return 0;
}