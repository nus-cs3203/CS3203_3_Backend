#ifndef ENV_MANAGER_HPP
#define ENV_MANAGER_HPP

#include <string>

#include "constants.hpp"

class EnvManager {
   public:
    EnvManager(const std::string& filename = Constants::ENV_FILE_DEFAULT_PATH);

    auto read_env(const std::string& key, const std::string& default_value) const -> std::string;

   private:
    void _load_env_file(const std::string& filename);
};

#endif  // ENV_MANAGER_HPP
