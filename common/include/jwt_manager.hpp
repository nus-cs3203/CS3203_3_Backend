#ifndef JWT_MANAGER_HPP
#define JWT_MANAGER_HPP

#include "constants.hpp"
#include "database_manager.hpp"
#include "env_manager.hpp"

#include "crow.h"

#include <functional>
#include <string>

enum class JwtAccessLevel {
    Personal,
    Admin,
    Citizen,
    None
};

class JwtManager {
public:
    JwtManager(
        const std::string& jwt_secret = env_manager.read_env("JWT_SECRET", Constants::DEFAULT_JWT_SECRET),
        const int& jwt_duration_in_seconds = stoi(env_manager.read_env("JWT_DURATION_IN_SECONDS", Constants::DEFAULT_JWT_DURATION_IN_SECONDS))
    );

    std::string generate_token(const std::string &oid, const std::string &role);
    
    auto jwt_protection_decorator(const std::function<crow::response(const crow::request&)>& func, const JwtAccessLevel& access_level) -> std::function<crow::response(const crow::request&)>;

private:
    std::string jwt_secret;
    int jwt_duration_in_seconds;

    std::string _get_from_token(const std::string &token, const std::string &key);
    
    static EnvManager env_manager;
};

#endif // JWT_MANAGER_HPP
