#ifndef MINI_BIOS_SHELL_HPP
#define MINI_BIOS_SHELL_HPP

#include <string>
#include <memory>
#include "../vpu/FiscVpu.hpp"
#include "../config/FiscConfigParser.hpp"

class MiniBiosShell {
public:
    MiniBiosShell(const FiscConfigParser& config);
    void run();
    
private:
    std::unique_ptr<FiscVpu> vpu;
    bool running;
    
    void processCommand(const std::string& cmd);
    void printHelp();
    void handleVpuOutput(const std::string& output);
};

#endif // MINI_BIOS_SHELL_HPP 