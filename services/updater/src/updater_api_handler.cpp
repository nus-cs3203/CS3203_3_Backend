#include "base_api_strategy_utils.hpp"
#include "constants.hpp"
#include "updater_api_handler.hpp"

#include <bsoncxx/json.hpp>
#include <cpr/cpr.h>  
#include "crow.h"
#include <mongocxx/exception/exception.hpp>

#include <string>
#include <tuple>
#include <unordered_map>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

UpdaterApiHandler::UpdaterApiHandler()
: reddit_manager(RedditManager::create_from_env()), env_manager(EnvManager()), analytics_url(env_manager.read_env("ANALYTICS_URL", Constants::DEFAULT_ANALYTICS_URL)) {}

auto UpdaterApiHandler::update_posts(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager
) -> crow::response {
    try {
        auto json_docs = reddit_manager->get_posts("singapore");
        std::vector<bsoncxx::document::value> bson_docs;
        for (auto &json_doc: json_docs) {
            auto json_doc_rval = crow::json::load(json_doc.dump());
            auto bson_doc = BaseApiStrategyUtils::parse_request_json_to_database_bson(json_doc_rval);
            bson_docs.push_back(std::move(bson_doc));
        }

        int successful_insertions = 0;
        int ignored_insertions = 0;
        int failed_insertions = 0;

        for (auto &bson_doc: bson_docs) {
            try {
                db_manager->insert_one(Constants::COLLECTION_POSTS, bson_doc);
                successful_insertions += 1;
            }
            catch (const mongocxx::exception& e) {
                if (e.code().value() == 11000) { 
                    // duplicate 
                    ignored_insertions += 1; 
                } else {
                    std::cout << "Failed to insert post to db: " << e.what() << std::endl;
                    failed_insertions += 1;
                }
            }
            catch (const std::exception& e) {
                std::cout << "Failed to insert post to db: " << e.what() << std::endl;
                failed_insertions += 1;
            }
        }
        
        crow::json::wvalue response_data;
        response_data["successful_insertions"] = successful_insertions;
        response_data["ignored_insertions"] = ignored_insertions;
        response_data["failed_insertions"] = failed_insertions;
        return BaseApiStrategyUtils::make_success_response(200, response_data, "Server processed update request successfully.");
    }
    catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto UpdaterApiHandler::run_analytics(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name
) -> crow::response {
    try {
        BaseApiStrategyUtils::validate_fields(req, {"start_date", "end_date"});

        std::unordered_map<std::string, std::string> URL_MAPPER = {
            {Constants::COLLECTION_COMPLAINTS, analytics_url + "/process_complaints"},
            {Constants::COLLECTION_CATEGORY_ANALYTICS, analytics_url + "/generate_category_analytics"},
            {Constants::COLLECTION_POLL_TEMPLATES, analytics_url + "/generate_poll_prompts"},
        };

        auto url = URL_MAPPER[collection_name];

        crow::json::wvalue body = crow::json::load(req.body);
        std::string body_str = body.dump();
        auto resp = cpr::Post(
            cpr::Url{url},
            cpr::Header{{"Content-Type", "application/json"}},
            cpr::Body{body_str}
        );

        auto analytics_resp_body = crow::json::load(resp.text);
        auto task_id_doc = make_document(
            kvp("task_id", analytics_resp_body["task_id"].s()),
            kvp("collection", collection_name)
        );
        db_manager->insert_one(Constants::COLLECTION_ANALYTICS_TASK_IDS, task_id_doc);

        crow::json::wvalue response_data;
        response_data["task_id"] = analytics_resp_body["task_id"];
        return BaseApiStrategyUtils::make_success_response(200, response_data, "Server processed update request successfully.");
    }
    catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto UpdaterApiHandler::retrieve_analytics(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager
) -> crow::response {
    try {
        auto task_id_cursor = db_manager->find(Constants::COLLECTION_ANALYTICS_TASK_IDS);

        std::unordered_map<std::string, std::string> BASE_URL_MAPPER = {
            {Constants::COLLECTION_COMPLAINTS, analytics_url + "/task_status"},
            {Constants::COLLECTION_CATEGORY_ANALYTICS, analytics_url + "/poll_generation_status"},
            {Constants::COLLECTION_POLL_TEMPLATES, analytics_url + "/category_analytics_status"},
        };

        for (auto&& doc: task_id_cursor) {
            auto doc_json = bsoncxx::to_json(doc);
            auto doc_rval = crow::json::load(doc_json);
            
            std::string task_id = doc_rval["task_id"].s();
            auto collection = doc_rval["collection"].s();
            auto url = BASE_URL_MAPPER[collection] + "/" + task_id;
            
            auto resp = cpr::Get(
                cpr::Url{url}
            );
            auto resp_json = crow::json::load(resp.text);
            auto resp_bson = BaseApiStrategyUtils::parse_request_json_to_database_bson(resp_json);
            try {
                db_manager->insert_one(collection, resp_bson);
                db_manager->delete_one(collection, doc);
            }
            catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
            }
        }

        crow::json::wvalue response_data;
        return BaseApiStrategyUtils::make_success_response(200, response_data, "Server processed update request successfully.");
    }
    catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto UpdaterApiHandler::clear_analytics(
    const crow::request& req, 
    std::shared_ptr<DatabaseManager> db_manager,
    const std::string& collection_name
) -> crow::response {
    try {
        auto result = db_manager->delete_many(collection_name, {});
        crow::json::wvalue response_data;
        auto deleted_count = result.value().deleted_count();
        response_data["deleted_count"] = deleted_count;
        return BaseApiStrategyUtils::make_success_response(200, response_data, "Server processed update request successfully.");
    }
    catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500, std::string("Server error: ") + e.what());
    }
}