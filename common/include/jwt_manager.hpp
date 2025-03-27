#ifndef JWT_MANAGER_HPP
#define JWT_MANAGER_HPP

#include "constants.hpp"
#include "database_manager.hpp"
#include "env_manager.hpp"

#include "crow.h"

#include <string>

using ApiHandler = std::function<crow::response(const crow::request&, std::shared_ptr<DatabaseManager>, const std::string&)>;

enum class JwtAccessLevel {
    Personal,
    Admin,
    Citizen
};

class JwtManager {
public:
    JwtManager(
        const std::string& jwt_secret = env_manager.read_env("JWT_SECRET", "")
    );

    std::string generate_token(const std::string &oid, const std::string &role);
    
    ApiHandler api_path_protection_decorator(const ApiHandler& api_handler, const crow::request& req, const JwtAccessLevel& access_level);

private:
    std::string jwt_secret;

    std::string _get_from_token(const std::string &token, const std::string &key);
    ApiHandler _create_error_api_handler(const int& status_code, const std::string& message);

    static EnvManager env_manager;
};

#endif // JWT_MANAGER_HPP
