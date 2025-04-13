#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>

void ConfigManager::loadConfig(const std::filesystem::path& configPath) {
    std::ifstream file(configPath);
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, value;
        
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            settings[key] = value;
        }
    }
}

void ConfigManager::saveConfig(const std::filesystem::path& configPath) {
    std::ofstream file(configPath);
    
    for (const auto& [key, value] : settings) {
        file << key << "=" << value << "\n";
    }
}

void ConfigManager::setValue(const std::string& key, const std::string& value) {
    settings[key] = value;
}

std::string ConfigManager::getValue(const std::string& key, const std::string& defaultValue) const {
    auto it = settings.find(key);
    return it != settings.end() ? it->second : defaultValue;
}