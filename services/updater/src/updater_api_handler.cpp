#include "base_api_strategy_utils.hpp"
#include "constants.hpp"
#include "updater_api_handler.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/exception/exception.hpp>

#include <string>
#include <tuple>

UpdaterApiHandler::UpdaterApiHandler()
: reddit_manager(RedditManager::create_from_env()) {}

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
        return BaseApiStrategyUtils::make_success_response(200, response_data, "Server processed insert request successfully.");
    }
    catch (const std::exception& e) {
        return BaseApiStrategyUtils::make_error_response(500, std::string("Server error: ") + e.what());
    }
}