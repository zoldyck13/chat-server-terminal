#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

#include "ClientSocket.hpp"

using namespace ftxui;

class ChatUI {
private:
    Component layout;
    Component input_box;
    Component container;
    std::string input;

public:
    std::function<void()> onQuit;

    ChatUI(ScreenInteractive& screen) {
        auto& client = ClientSocket::getInstace();

        client.setOnChatUpdate([&] {
            screen.PostEvent(Event::Custom);
        });

        input_box = Input(&input, "Type message...");

        input_box = CatchEvent(input_box, [&](Event e) {
            if (e == Event::Return && !input.empty()) {

                if (input == "/quit") {
                    client.stopReceiver();
                    input.clear();
                    if (onQuit) onQuit();
                    return true;
                }

                client.sendChat(input);

                input.clear();
                return true;
            }
            return false;
        });


        container = Container::Vertical({ input_box });

        layout = Renderer(container, [&] {
            auto messages = client.getChatState().getMessages();

            std::vector<Element> elems;
            elems.reserve(messages.size());
            for (auto& m : messages)
                elems.push_back(paragraph(m));

            bool focused = input_box->Focused();

            std::string shown_input = input;
            if (focused)
                shown_input += "▌";

            return vbox({
                text(" Chat Room ") | bold | center,
                separator(),

                vbox(elems)
                    |size(WIDTH, LESS_THAN, 80)
                    | frame
                    | vscroll_indicator
                    | size(HEIGHT, GREATER_THAN, 15)
                    | border,

                separator(),

                hbox({
                    text("> "),
                    text(shown_input) | flex,
                    text("  "),
                    text("[Enter to send | /quit]")
                })
                | border
            });
        });
    }

    Component Render() { return layout; }

    void FocusInput() { input_box->TakeFocus(); }
};
