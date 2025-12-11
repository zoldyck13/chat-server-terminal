#include "../include/client/Login.hpp"
#include <ftxui/component/component.hpp>
#include <thread>


int main() {

    ScreenInteractive screen = ScreenInteractive::Fullscreen();
    InitializeDb();

    int page = 0;

    Login login_page;

    Component container = Container::Tab({
        login_page.RenderLogin(),

    }, &page);


    login_page.onLog = [&]{
        page =1;

        screen.PostEvent(Event::Custom);
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