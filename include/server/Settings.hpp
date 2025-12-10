#ifndef __FTXUI_SETTINGS
#define __FTXUI_SETTINGS

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>

using namespace ftxui;

class Settingsinfo{

    private:

        Component layout;
        Component back_button;
        Component account_settings_menu;
        Component network_settings_menu;
        int selected_account = 0;
        int selected_network = 0;
        MenuOption option_account;
        MenuOption option_network;
        

        //Create the entries
        std::vector<std::string> account_settings_entries = {
            "Change username",
            "Change password",
            "Two-step verification (just display “coming soon”)",
            "Delete account (confirmation popup)",
        };

        std::vector<std::string> network_settings_entries = {
            "Server IP",
            "Server Port",
            "Auto-reconnect ON/OFF",
            "Connection timeout (ms)"
        };


    
    public:
        
        //Define the callback functions
        std::function<void()> onBack;
        std::function<void(int)> onSelectAccount;
        std::function<void(int)>  onSelectNetwork;


        Settingsinfo(){

            //Create Menu Options 
            option_account = MenuOption();
            option_account.on_enter = [&]{
                if(onSelectAccount) onSelectAccount(selected_account);
            };

            option_network = MenuOption();
            option_network.on_enter = [&] {
                if(onSelectNetwork) onSelectNetwork(selected_network);
            };
            
            //Create button
            back_button = Button("Back", [&]{
                if(onBack) onBack();
            }, ButtonOption::Simple());

            //Create the Menu
            account_settings_menu = Menu(&account_settings_entries, &selected_account, option_account);
            network_settings_menu = Menu(&network_settings_entries, &selected_network, option_network);

            //Pass it to contaier 
            auto container = Container::Vertical({
                account_settings_menu,
                network_settings_menu,
                back_button
            });


            //Render the layout 
            layout = Renderer(container, [&]{
                return vbox({
                        
                        vbox(
                            text("🔑 Account Settings") | border  | color(Color::Cyan),
                            separator(),
                            account_settings_menu->Render()
                        ) | border  | color(Color::Cyan),

                        separator(),

                        vbox({
                            text("🌐 Connection Settings") | border | color(Color::Cyan),
                            separator(),
                            network_settings_menu->Render() 
                        }) | border  | color(Color::Cyan),

                        hbox(text(""), back_button->Render()) | color(Color::Cyan),
                }) | center | vcenter;
            });
        }


        Component RenderSettings(){return layout;}


};




#endif