#include "FiscConfigTui.hpp"
#include <algorithm>

FiscConfigTui::FiscConfigTui() : selected(0) {
    menuItems = {MenuItem::LOAD, MenuItem::EDIT, MenuItem::SAVE, MenuItem::RUN, MenuItem::EXIT};
    initializeScreen();
}

FiscConfigTui::~FiscConfigTui() {
    delwin(mainwin);
    endwin();
}

void FiscConfigTui::initializeScreen() {
    mainwin = initscr();
    start_color();
    cbreak();
    noecho();
    keypad(mainwin, TRUE);
    curs_set(0);
    
    // Initialize color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLUE);  // Selected item
    init_pair(2, COLOR_WHITE, COLOR_BLACK); // Normal item
}

void FiscConfigTui::drawMenu() {
    clear();
    
    // Draw title
    attron(A_BOLD);
    mvprintw(1, 2, "FISC-V Configuration Manager (TUI)");
    attroff(A_BOLD);
    
    // Draw current file info
    mvprintw(3, 2, "Current file: %s", 
             currentFile.empty() ? "[None]" : currentFile.c_str());
    
    // Draw menu items
    const char* menuLabels[] = {"Load Config", "Edit Config", "Save Config", "Run VPU", "Exit"};
    
    for (size_t i = 0; i < menuItems.size(); ++i) {
        if (i == static_cast<size_t>(selected)) {
            attron(COLOR_PAIR(1));
            mvprintw(5 + i, 2, "> %s", menuLabels[i]);
            attroff(COLOR_PAIR(1));
        } else {
            attron(COLOR_PAIR(2));
            mvprintw(5 + i, 2, "  %s", menuLabels[i]);
            attroff(COLOR_PAIR(2));
        }
    }
    
    // Draw help text
    mvprintw(LINES - 2, 2, "Use arrow keys to navigate, Enter to select");
    
    refresh();
}

void FiscConfigTui::handleInput() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
            selected = (selected - 1 + menuItems.size()) % menuItems.size();
            break;
        case KEY_DOWN:
            selected = (selected + 1) % menuItems.size();
            break;
        case 10: // Enter key
            switch (menuItems[selected]) {
                case MenuItem::LOAD:
                    loadConfig();
                    break;
                case MenuItem::EDIT:
                    editConfig();
                    break;
                case MenuItem::SAVE:
                    saveConfig();
                    break;
                case MenuItem::RUN:
                    runVpu();
                    break;
                case MenuItem::EXIT:
                    // Exit will be handled in the run loop
                    break;
            }
            break;
    }
}

void FiscConfigTui::loadConfig() {
    std::string filename = getInput("Enter filename to load: ");
    if (!filename.empty()) {
        if (configParser.loadConfig(filename)) {
            currentFile = filename;
            showMessage("Configuration loaded successfully");
        } else {
            showMessage("Failed to load configuration");
        }
    }
}

void FiscConfigTui::editConfig() {
    if (currentFile.empty()) {
        showMessage("No configuration file loaded");
        return;
    }
    
    std::string param = getInput("Enter parameter name: ");
    if (param.empty()) return;
    
    std::string value = getInput("Enter new value: ");
    if (value.empty()) return;
    
    if (configParser.setParameter(param, value)) {
        showMessage("Parameter updated successfully");
    } else {
        showMessage("Failed to update parameter");
    }
}

void FiscConfigTui::saveConfig() {
    if (currentFile.empty()) {
        showMessage("No configuration file loaded");
        return;
    }
    
    if (configParser.saveConfig(currentFile)) {
        showMessage("Configuration saved successfully");
    } else {
        showMessage("Failed to save configuration");
    }
}

void FiscConfigTui::runVpu() {
    if (currentFile.empty()) {
        showMessage("No configuration file loaded");
        return;
    }
    
    // Temporarily exit ncurses mode
    endwin();
    
    // Create and run the shell
    MiniBiosShell shell(configParser);
    shell.run();
    
    // Restore ncurses mode
    refresh();
    initializeScreen();
}

std::string FiscConfigTui::getInput(const std::string& prompt) {
    echo();
    curs_set(1);
    
    // Clear bottom of screen for input
    move(LINES - 3, 0);
    clrtoeol();
    mvprintw(LINES - 3, 2, "%s", prompt.c_str());
    
    char input[256];
    getstr(input);
    
    noecho();
    curs_set(0);
    
    return std::string(input);
}

void FiscConfigTui::showMessage(const std::string& message) {
    mvprintw(LINES - 3, 2, "%s", message.c_str());
    refresh();
    napms(1500); // Show message for 1.5 seconds
}

void FiscConfigTui::run() {
    while (true) {
        drawMenu();
        handleInput();
        if (menuItems[selected] == MenuItem::EXIT) {
            break;
        }
    }
} 