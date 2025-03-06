#include "env_manager.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

EnvManager::EnvManager(const std::string& filename) {
    _load_env_file(filename);
}

auto EnvManager::read_env(const std::string& key, const std::string& default_value) const -> std::string {
    const char* env_var = std::getenv(key.c_str());
    if (!env_var) {
        std::cout << "[EnvManager] Environment variable '" << key << "' not found. Using default value." << std::endl;
        return default_value;
    }
    return static_cast<std::string>(env_var);
}

void EnvManager::_load_env_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[EnvManager] Could not find or open .env file at: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; 

        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            setenv(key.c_str(), value.c_str(), 1); 
        }
    }
    file.close();
}