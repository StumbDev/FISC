#include "FiscVpu.hpp"
#include <iostream>
#include <thread>
#include <chrono>

FiscVpu::FiscVpu(const FiscConfigParser& config)
    : config(config), running(false), pc(0) {
    std::fill(registers, registers + 32, 0);
}

bool FiscVpu::initialize() {
    try {
        // Get memory size from config
        auto memSize = std::stoul(config.getParameter("MEMORY_SIZE"));
        memory.resize(memSize);
        
        // Initialize other parameters from config
        pc = std::stoul(config.getParameter("START_ADDRESS"));
        
        loadMiniBios();
        
        if (!validateArchitectureConfig()) {
            if (outputCallback) {
                outputCallback("Invalid architecture configuration");
            }
            return false;
        }
        
        initializeArchitecture();
        return true;
    } catch (const std::exception& e) {
        if (outputCallback) {
            outputCallback("VPU initialization failed: " + std::string(e.what()));
        }
        return false;
    }
}

void FiscVpu::loadMiniBios() {
    // Simple mini BIOS implementation
    const uint8_t miniBios[] = {
        // Basic BIOS code - this is a placeholder
        0x13, 0x00, 0x00, 0x00,  // nop
        0x13, 0x00, 0x00, 0x00,  // nop
        0x93, 0x08, 0x10, 0x00,  // addi x17, x0, 1
        0x73, 0x00, 0x00, 0x00   // ecall (system call)
    };
    
    // Copy mini BIOS to beginning of memory
    std::copy(miniBios, miniBios + sizeof(miniBios), memory.begin());
}

bool FiscVpu::start() {
    if (running) return false;
    
    running = true;
    if (outputCallback) {
        outputCallback("VPU started");
    }
    
    // Start execution in a separate thread
    std::thread([this]() {
        while (running) {
            executeInstruction();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }).detach();
    
    return true;
}

void FiscVpu::stop() {
    running = false;
    if (outputCallback) {
        outputCallback("VPU stopped");
    }
}

void FiscVpu::executeInstruction() {
    // Simple instruction execution simulation
    if (pc >= memory.size() - 4) {
        stop();
        return;
    }
    
    // Fetch instruction (simplified)
    uint32_t instruction = 
        (memory[pc] << 24) |
        (memory[pc + 1] << 16) |
        (memory[pc + 2] << 8) |
        memory[pc + 3];
    
    // Execute instruction (simplified)
    if (instruction == 0x00000013) {  // nop
        if (outputCallback) {
            outputCallback("Executing NOP");
        }
    } else if ((instruction & 0x7F) == 0x73) {  // ecall
        if (outputCallback) {
            outputCallback("System call executed");
        }
        stop();
    }
    
    pc += 4;
}

void FiscVpu::initializeArchitecture() {
    std::string arch = config.getParameter("ARCHITECTURE");
    
    // Initialize architecture-specific state
    archState.realMode = config.getParameter("X86_REAL_MODE") == "true";
    archState.protectedMode = config.getParameter("X86_PROTECTED_MODE") == "true";
    archState.segmentation = config.getParameter("SEGMENT_REGISTERS") == "true";
    archState.fpuType = config.getParameter("FPU_TYPE");
    archState.mmuType = config.getParameter("MMU_TYPE");
    archState.simdSupport = config.getParameter("SIMD_SUPPORT");
    archState.virtualizationEnabled = config.getParameter("VIRTUALIZATION_SUPPORT") == "true";
    archState.clockMultiplier = std::stoul(config.getParameter("CLOCK_MULTIPLIER"));
    archState.waitStates = std::stoul(config.getParameter("WAIT_STATES"));
    
    // Initialize segment registers for x86 architectures
    if (arch.find("80") == 0 || arch == "Pentium") {
        archState.cs = archState.ds = archState.es = 
        archState.fs = archState.gs = archState.ss = 0;
    }
    
    if (outputCallback) {
        outputCallback("Initialized " + arch + " architecture");
        if (archState.realMode) {
            outputCallback("Running in real mode");
        }
        if (archState.protectedMode) {
            outputCallback("Protected mode enabled");
        }
    }
}

bool FiscVpu::validateArchitectureConfig() {
    std::string arch = config.getParameter("ARCHITECTURE");
    
    // Validate architecture-specific configurations
    if (arch.find("80") == 0 || arch == "Pentium") {
        // x86 family validation
        if (arch == "8086" || arch == "80186") {
            if (config.getParameter("X86_PROTECTED_MODE") == "true") {
                return false; // These CPUs don't support protected mode
            }
        }
        
        if (archState.realMode && archState.protectedMode) {
            return false; // Can't be in both modes simultaneously
        }
    }
    
    // Validate FPU configuration
    if (archState.fpuType != "none") {
        if (arch == "8086" && archState.fpuType != "8087") return false;
        if (arch == "80286" && archState.fpuType != "80287") return false;
        if (arch == "80386" && archState.fpuType != "80387") return false;
    }
    
    // Validate SIMD support
    if (archState.simdSupport != "none") {
        if (arch != "Pentium") return false; // Only Pentium and later support SIMD
    }
    
    return true;
}

bool FiscVpu::setConfigParameter(const std::string& param, const std::string& value) {
    if (!running) {
        return config.setParameter(param, value);
    }
    if (outputCallback) {
        outputCallback("Cannot modify configuration while VPU is running");
    }
    return false;
} 