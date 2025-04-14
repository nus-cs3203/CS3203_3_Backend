#include "user_api_handler.hpp"

#include <bsoncxx/json.hpp>
#include <string>
#include <tuple>

#include "base_api_strategy_utils.hpp"
#include "crow.h"
#include "management_api_strategy.hpp"
#include "user_api_strategy.hpp"

auto UserApiHandler::login(const crow::request& req, std::shared_ptr<DatabaseManager> db_manager,
                           const std::string& collection_name) -> crow::response {
    try {
        auto filter_and_option = UserApiStrategy::process_request_func_login(req);
        auto filter = std::get<0>(filter_and_option);
        auto option = std::get<1>(filter_and_option);

        auto result = db_manager->find_one(collection_name, filter, option);

        if (!result.has_value()) {
            return BaseApiStrategyUtils::make_success_response(
                200, {},
                "Server processed get request successfully but no matching documents found");
        }

        auto response_data = UserApiStrategy::process_response_func_login(result.value(), req);

        return BaseApiStrategyUtils::make_success_response(
            200, response_data, "Server processed get request successfully.");
    } catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500,
                                                         std::string("Server error: ") + e.what());
    }
}

auto UserApiHandler::get_one_profile_by_oid(const crow::request& req,
                                            std::shared_ptr<DatabaseManager> db_manager,
                                            const std::string& collection_name) -> crow::response {
    return find_one(req, db_manager, collection_name,
                    ManagementApiStrategy::process_request_func_get_one_by_oid,
                    UserApiStrategy::process_response_func_get_one_profile_by_oid);
}

auto UserApiHandler::insert_one_account_admin(const crow::request& req,
                                              std::shared_ptr<DatabaseManager> db_manager,
                                              const std::string& collection_name)
    -> crow::response {
    return insert_one(req, db_manager, collection_name,
                      UserApiStrategy::process_request_func_create_account_admin,
                      BaseApiStrategy::process_response_func_insert_one);
}

auto UserApiHandler::insert_one_account_citizen(const crow::request& req,
                                                std::shared_ptr<DatabaseManager> db_manager,
                                                const std::string& collection_name)
    -> crow::response {
    return insert_one(req, db_manager, collection_name,
                      UserApiStrategy::process_request_func_create_account_citizen,
                      BaseApiStrategy::process_response_func_insert_one);
}