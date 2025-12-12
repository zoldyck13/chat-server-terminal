#include "../include/client/Login.hpp"
#include <ftxui/component/component.hpp>
#include "../include/client/Register.hpp"
#include "../include/client/Menu.hpp"
#include "../include/client/ClientSocket.hpp"
#include <thread>


int main() {


    auto& client = ClientSocket::getInstace();

    if (!client.connectToServer()) {
        std::cerr << "Cannot connect to server\n";
        return 1;
    }

    ScreenInteractive screen = ScreenInteractive::Fullscreen();
    int page = 0;

    Login login_page;
    Register register_page;
    ClientMenu menu_page;

    Component container = Container::Tab({
        login_page.RenderLogin(),
        register_page.RenderRegister(),
        menu_page.RenderMenu(),

    }, &page);


    login_page.onLog = [&]{
        page =2;

        menu_page.setUsername(
            ClientSocket::getInstace().getUsername()
        );
                screen.PostEvent(Event::Custom);
    };

    login_page.onRegister = [&] {
        page = 1;

        screen.PostEvent(Event::Custom);
    };

    register_page.onBack = [&] {
        page = 0;

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