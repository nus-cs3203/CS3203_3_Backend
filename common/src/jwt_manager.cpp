#include "jwt_manager.hpp"

#include <jwt-cpp/jwt.h>

#include <chrono>
#include <stdexcept>

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

std::string JwtManager::get_oid_from_token(const std::string &token) {
    return _get_from_token(token, "oid");
}

std::string JwtManager::get_role_from_token(const std::string &token) {
    return _get_from_token(token, "role");
}

void JwtManager::validate_oid(const std::string &token, const std::string &expected_oid) {
    if (get_oid_from_token(token) != expected_oid) {
        throw std::runtime_error("Oid retrieved from JWT token is not the expected oid.");
    }
}

void JwtManager::validate_role(const std::string &token, const std::string &expected_role) {
    if (get_role_from_token(token) != expected_role) {
        throw std::runtime_error("Role retrieved from JWT token is not the expected role");
    }
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

EnvManager JwtManager::env_manager = EnvManager();

JwtManager::JwtManager(
    const std::string& jwt_secret
) : jwt_secret{jwt_secret} {}