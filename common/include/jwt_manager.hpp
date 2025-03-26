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

    std::string generate_token(const std::string &oid, const std::string &role);
    std::string get_oid_from_token(const std::string &token);
    std::string get_role_from_token(const std::string &token);
    void validate_oid(const std::string &token, const std::string &expected_oid);
    void validate_role(const std::string &token, const std::string &expected_role);
    
    // template <typename ApiHandler>
    // auto jwt_protect(ApiHandler api_handler, const std::string &required_role);

private:
    std::string jwt_secret;

    std::string _get_from_token(const std::string &token, const std::string &key);

    static EnvManager env_manager;
};

#endif // JWT_MANAGER_HPP
