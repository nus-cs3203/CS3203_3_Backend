#ifndef JWT_MANAGER_HPP
#define JWT_MANAGER_HPP

#include "constants.hpp"
#include "env_manager.hpp"

#include <string>

class JwtManager {
public:
    JwtManager(
        const std::string& jwt_secret = env_manager.read_env("JWT_SECRET", "")
    );

    std::string generate_token(const std::string &role);

    std::string get_user_role_from_token(const std::string &token);

private:
    std::string jwt_secret;

    static EnvManager env_manager;
};

#endif // JWT_MANAGER_HPP
