#ifndef FISC_CONFIG_PARSER_HPP
#define FISC_CONFIG_PARSER_HPP

#include <string>
#include <map>
#include <stdexcept>
#include "FiscConfigSchema.hpp"

class FiscConfigParser {
public:
    FiscConfigParser();
    
    bool loadConfig(const std::string& filename);
    bool saveConfig(const std::string& filename) const;
    bool setParameter(const std::string& param, const std::string& value);
    std::string getParameter(const std::string& param) const;
    
    // Returns all parameters as a map
    std::map<std::string, std::string> getAllParameters() const;
    
    // New methods for schema-based configuration
    static std::vector<std::string> getAvailableParameters();
    static std::string getParameterDescription(const std::string& param);
    static std::vector<std::string> getEnumValues(const std::string& param);
    bool initializeDefaults();

private:
    std::map<std::string, std::string> configData;
    
    // Helper methods for parsing FXml format
    bool parseLine(const std::string& line);
    std::string formatLine(const std::string& param, const std::string& value) const;
    bool validateValue(const std::string& param, const std::string& value) const;
};

#endif // FISC_CONFIG_PARSER_HPP 