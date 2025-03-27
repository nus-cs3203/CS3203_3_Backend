#include "base_api_strategy_utils.hpp"
#include "jwt_manager.hpp"

#include "crow.h"
#include <jwt-cpp/jwt.h>

#include <chrono>
#include <stdexcept>

JwtManager::JwtManager(
    const std::string& jwt_secret
) : jwt_secret{jwt_secret} {}

EnvManager JwtManager::env_manager = EnvManager();

std::string JwtManager::generate_token(const std::string &oid, const std::string &role) {
    auto token = jwt::create()
        .set_issuer("auth0")
        .set_type("JWS")
        .set_payload_claim("oid", jwt::claim(oid))
        .set_payload_claim("role", jwt::claim(role))
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(1))
        .sign(jwt::algorithm::hs256{jwt_secret});
    
    return token;
}

ApiHandler JwtManager::api_path_protection_decorator(const ApiHandler& api_handler, const crow::request& req, const JwtAccessLevel& access_level) {
    auto auth_header = req.get_header_value("Authorization");
    if (auth_header.empty() || auth_header.substr(0, 7) != "Bearer ") {
        return _create_error_api_handler(401, "Unauthorized: Missing or invalid Authorization header");
    }
    std::string token = auth_header.substr(7);
    try {
        auto oid_from_token = _get_from_token(token, "oid");
        auto role_from_token = _get_from_token(token, "role");

        switch (access_level) {
            case JwtAccessLevel::Personal: {
                auto body = crow::json::load(req.body);
                auto oid_from_request = static_cast<std::string>(body["oid"]);
                if (oid_from_token != oid_from_request) {
                    throw std::runtime_error("Invalid Personal Level Access: oid retrieved from JWT token is not the oid provided in the request.");
                }
                break;
            }
        
            case JwtAccessLevel::Admin: {
                if (role_from_token != "Admin") {
                    throw std::runtime_error("Invalid Admin Level Access: role retrieved from JWT token is below Admin.");
                }
                break;
            }
        
            case JwtAccessLevel::Citizen: {
                break;
            }
        }        

    } catch (const std::exception &ex) {
        return _create_error_api_handler(401, std::string("Unauthorized: ") + ex.what());
    }

    return api_handler;
}

std::string JwtManager::_get_from_token(const std::string &token, const std::string &key) {
    try {
        auto decoded = jwt::decode(token);

        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{jwt_secret})
            .with_issuer("auth0");
        verifier.verify(decoded);

        auto value = decoded.get_payload_claim(key).as_string();
        return value;
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Token verification failed: ") + e.what());
    }
}

ApiHandler JwtManager::_create_error_api_handler(const int& status_code, const std::string& message) {
    return [status_code, message](const crow::request& req, std::shared_ptr<DatabaseManager> db, const std::string& user_id) -> crow::response {
        return BaseApiStrategyUtils::make_error_response(status_code, message);
    };
}