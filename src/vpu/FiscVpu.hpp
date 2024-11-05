#ifndef FISC_VPU_HPP
#define FISC_VPU_HPP

#include <string>
#include <memory>
#include <functional>
#include "../config/FiscConfigParser.hpp"

class FiscVpu {
public:
    FiscVpu(const FiscConfigParser& config);
    
    bool initialize();
    bool start();
    void stop();
    bool isRunning() const { return running; }
    
    // Register a callback for BIOS output
    void setOutputCallback(std::function<void(const std::string&)> callback) {
        outputCallback = callback;
    }
    
    // Configuration methods
    const FiscConfigParser& getConfig() const { return config; }
    bool setConfigParameter(const std::string& param, const std::string& value);

private:
    FiscConfigParser config;  // Now owned by VPU, not a reference
    bool running;
    std::function<void(const std::string&)> outputCallback;
    
    // VPU state
    uint32_t registers[32];
    uint32_t pc;
    std::vector<uint8_t> memory;
    
    void executeInstruction();
    void loadMiniBios();
    
    struct ArchitectureState {
        bool realMode;
        bool protectedMode;
        bool segmentation;
        std::string fpuType;
        std::string mmuType;
        std::string simdSupport;
        bool virtualizationEnabled;
        uint32_t clockMultiplier;
        uint32_t waitStates;
        
        // Segment registers (for x86)
        uint16_t cs, ds, es, fs, gs, ss;
    };
    
    ArchitectureState archState;
    void initializeArchitecture();
    bool validateArchitectureConfig();
};

#endif // FISC_VPU_HPP 