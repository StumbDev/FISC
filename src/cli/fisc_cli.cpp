#include "../config/FiscConfigParser.hpp"
#include "../shell/MiniBiosShell.hpp"
#include <iostream>
#include <string>

void printUsage() {
    std::cout << "Usage:\n"
              << "  load <filename>                        - Load and display configuration\n"
              << "  edit <filename> <parameter> <value>    - Edit parameter in configuration\n"
              << "  save <filename>                        - Save configuration\n"
              << "  run <filename>                         - Run VPU with configuration\n"
              << "  exit                                   - Exit the program\n";
}

int main(int argc, char* argv[]) {
    FiscConfigParser parser;
    std::string command, filename, param, value;

    std::cout << "FISC-V Configuration CLI\n";
    printUsage();

    while (true) {
        std::cout << "\nEnter command: ";
        std::cin >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "run") {
            std::cin >> filename;
            if (parser.loadConfig(filename)) {
                std::cout << "Starting MiniBIOS shell...\n";
                MiniBiosShell shell(parser);
                shell.run();
            } else {
                std::cout << "Error loading configuration.\n";
            }
        }
        else if (command == "load") {
            std::cin >> filename;
            if (parser.loadConfig(filename)) {
                std::cout << "Configuration loaded successfully.\n";
                auto params = parser.getAllParameters();
                for (const auto& [param, value] : params) {
                    std::cout << param << " = " << value << "\n";
                }
            } else {
                std::cout << "Error loading configuration.\n";
            }
        }
        else if (command == "edit") {
            std::cin >> filename >> param >> value;
            if (parser.loadConfig(filename)) {
                parser.setParameter(param, value);
                std::cout << "Parameter updated.\n";
            } else {
                std::cout << "Error loading configuration for editing.\n";
            }
        }
        else if (command == "save") {
            std::cin >> filename;
            if (parser.saveConfig(filename)) {
                std::cout << "Configuration saved successfully.\n";
            } else {
                std::cout << "Error saving configuration.\n";
            }
        }
        else {
            printUsage();
        }
    }

    return 0;
} 