#include "MiniBiosShell.hpp"
#include <iostream>
#include <sstream>

MiniBiosShell::MiniBiosShell(const FiscConfigParser& config)
    : running(false) {
    vpu = std::make_unique<FiscVpu>(config);
    vpu->setOutputCallback([this](const std::string& output) {
        handleVpuOutput(output);
    });
}

void MiniBiosShell::run() {
    running = true;
    std::cout << "FISC-V MiniBIOS Shell\n";
    std::cout << "Type 'help' for available commands\n";
    
    while (running) {
        std::cout << "minibios> ";
        std::string input;
        std::getline(std::cin, input);
        processCommand(input);
    }
}

void MiniBiosShell::processCommand(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string command;
    iss >> command;
    
    if (command == "help") {
        printHelp();
    }
    else if (command == "show" && iss >> command && command == "config") {
        auto params = vpu->getConfig().getAllParameters();
        std::cout << "Current configuration:\n";
        for (const auto& [param, value] : params) {
            std::cout << param << " = " << value << "\n";
        }
    }
    else if (command == "set") {
        std::string param, value;
        if (iss >> param >> value) {
            if (vpu->setConfigParameter(param, value)) {
                std::cout << "Parameter updated successfully\n";
            } else {
                std::cout << "Invalid parameter or value\n";
            }
        }
    }
    else if (command == "info") {
        std::string param;
        if (iss >> param) {
            std::cout << "Parameter: " << param << "\n"
                     << "Description: " << FiscConfigParser::getParameterDescription(param) << "\n";
            auto enumValues = FiscConfigParser::getEnumValues(param);
            if (!enumValues.empty()) {
                std::cout << "Possible values: ";
                for (const auto& val : enumValues) {
                    std::cout << val << " ";
                }
                std::cout << "\n";
            }
        }
    }
    else if (command == "list" && iss >> command && command == "params") {
        auto params = FiscConfigParser::getAvailableParameters();
        std::cout << "Available parameters:\n";
        for (const auto& param : params) {
            std::cout << "  " << param << "\n";
        }
    }
    else if (command == "start") {
        if (!vpu->isRunning()) {
            if (vpu->initialize() && vpu->start()) {
                std::cout << "VPU started\n";
            } else {
                std::cout << "Failed to start VPU\n";
            }
        } else {
            std::cout << "VPU is already running\n";
        }
    }
    else if (command == "stop") {
        if (vpu->isRunning()) {
            vpu->stop();
            std::cout << "VPU stopped\n";
        } else {
            std::cout << "VPU is not running\n";
        }
    }
    else if (command == "exit") {
        if (vpu->isRunning()) {
            vpu->stop();
        }
        running = false;
    }
    else if (!command.empty()) {
        std::cout << "Unknown command. Type 'help' for available commands\n";
    }
}

void MiniBiosShell::printHelp() {
    std::cout << "Available commands:\n"
              << "  start           - Start the VPU\n"
              << "  stop            - Stop the VPU\n"
              << "  show config     - Display current configuration\n"
              << "  set <param> <value> - Set configuration parameter\n"
              << "  info <param>    - Show parameter information\n"
              << "  list params     - List all available parameters\n"
              << "  help            - Show this help message\n"
              << "  exit            - Exit the shell\n";
}

void MiniBiosShell::handleVpuOutput(const std::string& output) {
    std::cout << "[VPU] " << output << "\n";
} 