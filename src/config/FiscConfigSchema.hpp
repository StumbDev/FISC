#ifndef FISC_CONFIG_SCHEMA_HPP
#define FISC_CONFIG_SCHEMA_HPP

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <optional>

class FiscConfigSchema {
public:
    enum class ParamType {
        INTEGER,
        HEX,
        STRING,
        BOOLEAN,
        ENUM
    };

    struct ParamDefinition {
        ParamType type;
        std::string description;
        std::string defaultValue;
        std::vector<std::string> enumValues;  // Used for ENUM type
        std::function<bool(const std::string&)> validator;
    };

    static const std::map<std::string, ParamDefinition>& getSchema();
    static bool validateParameter(const std::string& param, const std::string& value);
    static std::optional<std::string> getDefaultValue(const std::string& param);
    static std::string getDescription(const std::string& param);

private:
    static std::map<std::string, ParamDefinition> createSchema();
    static const std::map<std::string, ParamDefinition> schema;
};

#endif // FISC_CONFIG_SCHEMA_HPP 