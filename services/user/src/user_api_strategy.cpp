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
    auto password = body["password"].s();

    auto filter = make_document(
        kvp("email", email),
        kvp("password", password)
    );

    mongocxx::options::find option;

    return std::make_tuple(filter, option);
}

auto UserApiStrategy::_process_request_func_create_account(const crow::request& req, const std::string& role) -> std::tuple<bsoncxx::document::value, mongocxx::options::insert> {
    BaseApiStrategyUtils::validate_fields(req, {"document"});

    auto body = crow::json::load(req.body);
    
    crow::json::wvalue account = body["document"];
    account["role"] = role;
    auto account_rval = crow::json::load(account.dump());
    auto document = BaseApiStrategyUtils::parse_request_json_to_database_bson(account_rval);
    mongocxx::options::insert option;

    return std::make_tuple(document, option);
}

auto UserApiStrategy::process_request_func_create_account_admin(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::insert> {
    return _process_request_func_create_account(req, Constants::USERS_ROLE_ADMIN);
}

auto UserApiStrategy::process_request_func_create_account_citizen(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::insert> {
    return _process_request_func_create_account(req, Constants::USERS_ROLE_CITIZEN);
}

auto UserApiStrategy::process_response_func_login(const bsoncxx::document::value& doc) -> crow::json::wvalue {
    auto document_json = bsoncxx::to_json(doc);
    auto document_rvalue = crow::json::load(document_json);
    auto oid = document_rvalue["_id"]["$oid"].s();
    
    JwtManager jwt_manager;
    auto role = static_cast<std::string>(document_rvalue["role"].s());
    auto jwt = jwt_manager.generate_token(role);

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