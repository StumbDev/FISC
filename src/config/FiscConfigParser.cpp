#include "FiscConfigParser.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

FiscConfigParser::FiscConfigParser() {
    initializeDefaults();
}

bool FiscConfigParser::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    configData.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (!parseLine(line)) {
            return false;
        }
    }
    return true;
}

bool FiscConfigParser::saveConfig(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& [param, value] : configData) {
        file << formatLine(param, value) << "\n";
    }
    return true;
}

bool FiscConfigParser::setParameter(const std::string& param, const std::string& value) {
    if (!validateValue(param, value)) {
        return false;
    }
    configData[param] = value;
    return true;
}

std::string FiscConfigParser::getParameter(const std::string& param) const {
    auto it = configData.find(param);
    if (it == configData.end()) {
        // Try to get default value from schema
        auto defaultValue = FiscConfigSchema::getDefaultValue(param);
        if (defaultValue) {
            return *defaultValue;
        }
        throw std::runtime_error("Parameter not found: " + param);
    }
    return it->second;
}

std::map<std::string, std::string> FiscConfigParser::getAllParameters() const {
    return configData;
}

bool FiscConfigParser::parseLine(const std::string& line) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#') {
        return true;
    }

    size_t equalPos = line.find('=');
    if (equalPos != std::string::npos) {
        std::string param = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // Trim whitespace
        param.erase(0, param.find_first_not_of(" \t"));
        param.erase(param.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        return setParameter(param, value);
    }
    return true;
}

std::string FiscConfigParser::formatLine(const std::string& param, const std::string& value) const {
    return param + " = " + value;
}

bool FiscConfigParser::validateValue(const std::string& param, const std::string& value) const {
    return FiscConfigSchema::validateParameter(param, value);
}

// Static methods
std::vector<std::string> FiscConfigParser::getAvailableParameters() {
    std::vector<std::string> params;
    for (const auto& [param, def] : FiscConfigSchema::getSchema()) {
        params.push_back(param);
    }
    return params;
}

std::string FiscConfigParser::getParameterDescription(const std::string& param) {
    return FiscConfigSchema::getDescription(param);
}

std::vector<std::string> FiscConfigParser::getEnumValues(const std::string& param) {
    const auto& schema = FiscConfigSchema::getSchema();
    auto it = schema.find(param);
    if (it != schema.end() && it->second.type == FiscConfigSchema::ParamType::ENUM) {
        return it->second.enumValues;
    }
    return {};
}

bool FiscConfigParser::initializeDefaults() {
    const auto& schema = FiscConfigSchema::getSchema();
    for (const auto& [param, def] : schema) {
        configData[param] = def.defaultValue;
    }
    return true;
} 