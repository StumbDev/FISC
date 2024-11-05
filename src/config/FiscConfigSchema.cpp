#include "FiscConfigSchema.hpp"
#include <regex>
#include <algorithm>

const std::map<std::string, FiscConfigSchema::ParamDefinition> FiscConfigSchema::schema = 
    FiscConfigSchema::createSchema();

std::map<std::string, FiscConfigSchema::ParamDefinition> FiscConfigSchema::createSchema() {
    std::map<std::string, ParamDefinition> s;
    
    // Memory configuration
    s["MEMORY_SIZE"] = {
        ParamType::INTEGER,
        "Total memory size in bytes (must be power of 2)",
        "65536",
        {},
        [](const std::string& val) {
            try {
                auto size = std::stoull(val);
                return (size & (size - 1)) == 0; // Check power of 2
            } catch (...) {
                return false;
            }
        }
    };

    s["START_ADDRESS"] = {
        ParamType::HEX,
        "Program start address (must be aligned to 4 bytes)",
        "0x0000",
        {},
        [](const std::string& val) {
            try {
                auto addr = std::stoull(val, nullptr, 16);
                return (addr & 0x3) == 0; // Check 4-byte alignment
            } catch (...) {
                return false;
            }
        }
    };

    // CPU Configuration
    s["CPU_FREQUENCY"] = {
        ParamType::INTEGER,
        "CPU frequency in Hz (1000-1000000000)",
        "1000000",
        {},
        [](const std::string& val) {
            try {
                auto freq = std::stoull(val);
                return freq >= 1000 && freq <= 1000000000;
            } catch (...) {
                return false;
            }
        }
    };

    s["PIPELINE_STAGES"] = {
        ParamType::ENUM,
        "Pipeline configuration",
        "5-stage",
        {"3-stage", "5-stage", "7-stage"},
        [](const std::string& val) {
            return val == "3-stage" || val == "5-stage" || val == "7-stage";
        }
    };

    // Cache Configuration
    s["ICACHE_SIZE"] = {
        ParamType::INTEGER,
        "Instruction cache size in bytes (power of 2)",
        "4096",
        {},
        [](const std::string& val) {
            try {
                auto size = std::stoull(val);
                return (size & (size - 1)) == 0;
            } catch (...) {
                return false;
            }
        }
    };

    s["DCACHE_SIZE"] = {
        ParamType::INTEGER,
        "Data cache size in bytes (power of 2)",
        "4096",
        {},
        [](const std::string& val) {
            try {
                auto size = std::stoull(val);
                return (size & (size - 1)) == 0;
            } catch (...) {
                return false;
            }
        }
    };

    // BIOS Configuration
    s["BIOS_ENABLE"] = {
        ParamType::BOOLEAN,
        "Enable BIOS initialization",
        "true",
        {},
        [](const std::string& val) {
            return val == "true" || val == "false";
        }
    };

    s["BIOS_LOCATION"] = {
        ParamType::HEX,
        "BIOS memory location",
        "0x0000",
        {},
        [](const std::string& val) {
            try {
                auto addr = std::stoull(val, nullptr, 16);
                return (addr & 0x3) == 0;
            } catch (...) {
                return false;
            }
        }
    };

    // Debug Options
    s["DEBUG_LEVEL"] = {
        ParamType::ENUM,
        "Debug output level",
        "normal",
        {"none", "minimal", "normal", "verbose", "debug"},
        [](const std::string& val) {
            std::vector<std::string> valid = {"none", "minimal", "normal", "verbose", "debug"};
            return std::find(valid.begin(), valid.end(), val) != valid.end();
        }
    };

    s["TRACE_INSTRUCTIONS"] = {
        ParamType::BOOLEAN,
        "Enable instruction tracing",
        "false",
        {},
        [](const std::string& val) {
            return val == "true" || val == "false";
        }
    };

    // Architecture Configuration
    s["ARCHITECTURE"] = {
        ParamType::ENUM,
        "CPU Architecture type",
        "RISC-V-32",
        {
            // RISC-V variants
            "RISC-V-32", "RISC-V-64", "RISC-V-128",
            // x86 family
            "8086", "80186", "80286", "80386", "80486", "Pentium",
            "80386SX", "80386DX", "80486SX", "80486DX", "80486DX2", "80486DX4",
            // 8-bit architectures
            "8080", "Z80", "6502", "6800",
            // 16-bit architectures
            "65816", "68000",
            // 4-bit architectures
            "Intel-4004", "Intel-4040"
        },
        [](const std::string& val) {
            const std::vector<std::string> valid = {
                "RISC-V-32", "RISC-V-64", "RISC-V-128",
                "8086", "80186", "80286", "80386", "80486", "Pentium",
                "80386SX", "80386DX", "80486SX", "80486DX", "80486DX2", "80486DX4",
                "8080", "Z80", "6502", "6800",
                "65816", "68000",
                "Intel-4004", "Intel-4040"
            };
            return std::find(valid.begin(), valid.end(), val) != valid.end();
        }
    };

    s["INSTRUCTION_SET_EXTENSIONS"] = {
        ParamType::STRING,
        "Enabled instruction set extensions (comma-separated)",
        "base",
        {},
        [](const std::string& val) {
            // Simple validation - could be enhanced
            return !val.empty();
        }
    };

    s["ADDRESS_BUS_WIDTH"] = {
        ParamType::ENUM,
        "Address bus width in bits",
        "32",
        {"4", "8", "16", "20", "24", "32", "64", "128"},
        [](const std::string& val) {
            const std::vector<std::string> valid = {"4", "8", "16", "20", "24", "32", "64", "128"};
            return std::find(valid.begin(), valid.end(), val) != valid.end();
        }
    };

    s["DATA_BUS_WIDTH"] = {
        ParamType::ENUM,
        "Data bus width in bits",
        "32",
        {"4", "8", "16", "32", "64", "128"},
        [](const std::string& val) {
            const std::vector<std::string> valid = {"4", "8", "16", "32", "64", "128"};
            return std::find(valid.begin(), valid.end(), val) != valid.end();
        }
    };

    s["ENDIANNESS"] = {
        ParamType::ENUM,
        "Memory byte order",
        "little",
        {"little", "big", "bi"},
        [](const std::string& val) {
            return val == "little" || val == "big" || val == "bi";
        }
    };

    // Architecture-specific features
    s["X86_REAL_MODE"] = {
        ParamType::BOOLEAN,
        "Enable x86 real mode (16-bit mode)",
        "false",
        {},
        [](const std::string& val) {
            return val == "true" || val == "false";
        }
    };

    s["X86_PROTECTED_MODE"] = {
        ParamType::BOOLEAN,
        "Enable x86 protected mode",
        "true",
        {},
        [](const std::string& val) {
            return val == "true" || val == "false";
        }
    };

    s["SEGMENT_REGISTERS"] = {
        ParamType::BOOLEAN,
        "Enable segmentation (for x86 architectures)",
        "false",
        {},
        [](const std::string& val) {
            return val == "true" || val == "false";
        }
    };

    s["FPU_TYPE"] = {
        ParamType::ENUM,
        "Floating-point unit type",
        "none",
        {"none", "8087", "80287", "80387", "80487", "internal"},
        [](const std::string& val) {
            const std::vector<std::string> valid = {"none", "8087", "80287", "80387", "80487", "internal"};
            return std::find(valid.begin(), valid.end(), val) != valid.end();
        }
    };

    s["MMU_TYPE"] = {
        ParamType::ENUM,
        "Memory Management Unit type",
        "none",
        {"none", "basic", "paging", "segmentation", "paging_and_segmentation"},
        [](const std::string& val) {
            const std::vector<std::string> valid = {
                "none", "basic", "paging", "segmentation", "paging_and_segmentation"
            };
            return std::find(valid.begin(), valid.end(), val) != valid.end();
        }
    };

    // Architecture Extensions
    s["SIMD_SUPPORT"] = {
        ParamType::ENUM,
        "SIMD instruction set support",
        "none",
        {"none", "MMX", "SSE", "SSE2", "SSE3", "SSSE3", "SSE4", "AVX"},
        [](const std::string& val) {
            const std::vector<std::string> valid = {
                "none", "MMX", "SSE", "SSE2", "SSE3", "SSSE3", "SSE4", "AVX"
            };
            return std::find(valid.begin(), valid.end(), val) != valid.end();
        }
    };

    s["VIRTUALIZATION_SUPPORT"] = {
        ParamType::BOOLEAN,
        "Enable hardware virtualization support",
        "false",
        {},
        [](const std::string& val) {
            return val == "true" || val == "false";
        }
    };

    // Add architecture-specific timing parameters
    s["CLOCK_MULTIPLIER"] = {
        ParamType::INTEGER,
        "CPU clock multiplier (1-100)",
        "1",
        {},
        [](const std::string& val) {
            try {
                auto mult = std::stoi(val);
                return mult >= 1 && mult <= 100;
            } catch (...) {
                return false;
            }
        }
    };

    s["WAIT_STATES"] = {
        ParamType::INTEGER,
        "Memory wait states (0-7)",
        "0",
        {},
        [](const std::string& val) {
            try {
                auto states = std::stoi(val);
                return states >= 0 && states <= 7;
            } catch (...) {
                return false;
            }
        }
    };

    return s;
}

const std::map<std::string, FiscConfigSchema::ParamDefinition>& FiscConfigSchema::getSchema() {
    return schema;
}

bool FiscConfigSchema::validateParameter(const std::string& param, const std::string& value) {
    auto it = schema.find(param);
    if (it == schema.end()) {
        return false;
    }
    return it->second.validator(value);
}

std::optional<std::string> FiscConfigSchema::getDefaultValue(const std::string& param) {
    auto it = schema.find(param);
    if (it == schema.end()) {
        return std::nullopt;
    }
    return it->second.defaultValue;
}

std::string FiscConfigSchema::getDescription(const std::string& param) {
    auto it = schema.find(param);
    if (it == schema.end()) {
        return "Unknown parameter";
    }
    return it->second.description;
} 