#ifndef FISC_CONFIG_TUI_HPP
#define FISC_CONFIG_TUI_HPP

#include <ncurses.h>
#include <string>
#include <vector>
#include "../config/FiscConfigParser.hpp"

class FiscConfigTui {
public:
    FiscConfigTui();
    ~FiscConfigTui();
    
    void run();

private:
    enum class MenuItem {
        LOAD,
        EDIT,
        SAVE,
        RUN,
        EXIT
    };

    WINDOW* mainwin;
    FiscConfigParser configParser;
    std::string currentFile;
    int selected;
    std::vector<MenuItem> menuItems;
    
    void initializeScreen();
    void drawMenu();
    void handleInput();
    void loadConfig();
    void editConfig();
    void saveConfig();
    
    // Helper methods
    void showMessage(const std::string& message);
    std::string getInput(const std::string& prompt);
    void refreshScreen();
};

#endif // FISC_CONFIG_TUI_HPP 