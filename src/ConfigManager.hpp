#pragma once
#include <string>
#include <map>
#include <filesystem>

class ConfigManager {
public:
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    void loadConfig(const std::filesystem::path& configPath);
    void saveConfig(const std::filesystem::path& configPath);
    
    void setValue(const std::string& key, const std::string& value);
    std::string getValue(const std::string& key, const std::string& defaultValue = "") const;

private:
    ConfigManager() = default;
    std::map<std::string, std::string> settings;
};