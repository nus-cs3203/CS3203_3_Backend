#ifndef USER_API_STRATEGY_H
#define USER_API_STRATEGY_H

#include "database_manager.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <openssl/sha.h>

#include <random>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <vector>

namespace UserApiStrategy {
    auto process_request_func_login(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find>; 
    
    auto process_request_func_create_account_admin(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::insert>; 
    auto process_request_func_create_account_citizen(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::insert>;
    auto _process_request_func_create_account(const crow::request& req, const std::string& role) -> std::tuple<bsoncxx::document::value, mongocxx::options::insert>; 
    auto _generate_salt(size_t length = 16) -> std::string;
    auto _sha256(const std::string& input) -> std::string;

    auto process_response_func_login(const bsoncxx::document::value& doc, const crow::request& req) -> crow::json::wvalue;

    auto process_response_func_get_one_profile_by_oid(const bsoncxx::document::value& doc) -> crow::json::wvalue;
}

#endif