#include "api_handler.hpp"
#include "constants.hpp"
#include "database.hpp" 
#include "reddit.hpp" 
#include "utils.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <cpr/cpr.h>  
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <chrono>
#include <ctime>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

auto ApiHandler::perform_realtime_update_complaints_analytics_from_reddit(const crow::request& req, std::shared_ptr<Database> db, std::shared_ptr<Reddit> reddit, const std::string& subreddit, const std::string& collection_name) -> crow::response {
    try {
        auto utc_ts_now = get_utc_timestamp_now();
        auto utc_ts_one_day_ago = get_utc_timestamp_one_day_ago();
        auto posts = reddit->get_posts(subreddit, 1000, utc_ts_one_day_ago, utc_ts_now);
        auto complaints = reddit->get_complaints_from_posts(posts);

        std::vector<bsoncxx::document::value> complaint_docs;
        for (auto &complaint: complaints) {
            crow::json::rvalue complaint_rval = crow::json::load(complaint.dump());
            auto complaint_doc = json_to_bson(complaint_rval);
            complaint_docs.push_back(complaint_doc);
        }

        mongocxx::options::insert option;
        option.ordered(false); 
        db->insert_many(collection_name, complaint_docs, option);

        crow::json::wvalue response_data;
        return make_success_response(200, response_data, "Success");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}