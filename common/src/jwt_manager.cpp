#include "jwt_manager.hpp"

#include <jwt-cpp/jwt.h>

#include <chrono>
#include <stdexcept>

std::string JwtManager::generate_token(const std::string &role) {
    auto token = jwt::create()
        .set_issuer("auth0")
        .set_type("JWS")
        .set_payload_claim("role", jwt::claim(role))
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(1))
        .sign(jwt::algorithm::hs256{jwt_secret});
    
    return token;
}

std::string JwtManager::get_user_role_from_token(const std::string &token) {
    try {
        auto decoded = jwt::decode(token);

        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{jwt_secret})
            .with_issuer("auth0");
        verifier.verify(decoded);

        auto role = decoded.get_payload_claim("role").as_string();
        return role;
    } catch (const std::exception &e) {
        throw std::runtime_error(std::string("Token verification failed: ") + e.what());
    }
}

EnvManager JwtManager::env_manager = EnvManager();

JwtManager::JwtManager(
    const std::string& jwt_secret
) : jwt_secret{jwt_secret} {}