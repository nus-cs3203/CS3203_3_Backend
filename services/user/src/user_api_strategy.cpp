#include "base_api_strategy_utils.hpp"
#include "jwt_manager.hpp"
#include "user_api_strategy.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>
#include <tuple>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto UserApiStrategy::process_request_func_login(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find> {
    BaseApiStrategyUtils::validate_fields(req, {"email", "password"});

    auto body = crow::json::load(req.body);
    auto email = body["email"].s();

    auto filter = make_document(
        kvp("email", email)
    );

    mongocxx::options::find option;

    return std::make_tuple(filter, option);
}

auto UserApiStrategy::_process_request_func_create_account(const crow::request& req, const std::string& role) -> std::tuple<bsoncxx::document::value, mongocxx::options::insert> {
    BaseApiStrategyUtils::validate_fields(req, {"document"});

    auto body = crow::json::load(req.body);
    
    auto account_rval = body["document"];
    crow::json::wvalue account;

    account["name"] = account_rval["name"];
    account["email"] = account_rval["email"];
    account["collectibles"] = account_rval["collectibles"];
    account["role"] = role;
    
    std::string password = account_rval["password"].s();
    auto salt = _generate_salt();
    auto salted_password = salt + password;
    auto hashed_password = _sha256(salted_password);

    account["salt"] = salt;
    account["hashed_password"] = hashed_password;
    
    auto document = BaseApiStrategyUtils::parse_request_json_to_database_bson(crow::json::load(account.dump()));
    
    mongocxx::options::insert option;

    return std::make_tuple(document, option);
}

auto UserApiStrategy::process_request_func_create_account_admin(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::insert> {
    return _process_request_func_create_account(req, Constants::USERS_ROLE_ADMIN);
}

auto UserApiStrategy::process_request_func_create_account_citizen(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::insert> {
    return _process_request_func_create_account(req, Constants::USERS_ROLE_CITIZEN);
}

auto UserApiStrategy::_generate_salt(size_t length) -> std::string {
    const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t max_index = sizeof(charset) - 2; // -1 for '\0'
    std::random_device rd;   
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> dist(0, max_index);

    std::string salt;
    for (size_t i = 0; i < length; ++i) {
        salt += charset[dist(engine)];
    }
    return salt;
}

auto UserApiStrategy::_sha256(const std::string &input) -> std::string {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);
    
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
        
    return ss.str();
}

auto UserApiStrategy::process_response_func_login(const bsoncxx::document::value& doc, const crow::request& req) -> crow::json::wvalue {
    auto body = crow::json::load(req.body);
    std::string password = body["password"].s();

    auto document_json = bsoncxx::to_json(doc);
    auto document_rvalue = crow::json::load(document_json);

    std::string salt = document_rvalue["salt"].s();
    auto salted_password = salt + password;
    std::string hashed_password = _sha256(salted_password);
    std::string hashed_password_from_db = document_rvalue["hashed_password"].s();

    if (hashed_password != hashed_password_from_db) {
        throw std::runtime_error("Email and password do not match");
    }

    auto oid = document_rvalue["_id"]["$oid"].s();
    
    JwtManager jwt_manager;
    auto oid = static_cast<std::string>(document_rvalue["_id"]["$oid"].s());
    auto role = static_cast<std::string>(document_rvalue["role"].s());
    auto jwt = jwt_manager.generate_token(oid, role);

    crow::json::wvalue response_data;
    response_data["oid"] = oid;
    response_data["jwt"] = jwt;

    return response_data;
}

auto UserApiStrategy::process_response_func_get_one_profile_by_oid(const bsoncxx::document::value& doc) -> crow::json::wvalue {
    crow::json::wvalue profile;
    auto doc_json = bsoncxx::to_json(doc);
    auto doc_rval = crow::json::load(doc_json);
    auto parsed_doc_wval = BaseApiStrategyUtils::parse_database_json_to_response_json(doc_rval);
    auto parsed_doc_rval = crow::json::load(parsed_doc_wval.dump());
    profile["_id"] = parsed_doc_rval["_id"];
    profile["name"] = parsed_doc_rval["name"];
    profile["email"] = parsed_doc_rval["email"];
    profile["role"] = parsed_doc_rval["role"];
    profile["collectibles"] = parsed_doc_rval["collectibles"];
    
    crow::json::wvalue response_data;
    response_data["profile"] = std::move(profile);
    return response_data;
}