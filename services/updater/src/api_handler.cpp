// #include "api_handler.hpp"
// #include "constants.hpp"
// #include "database.hpp"
// #include "reddit.hpp"
// #include "utils.hpp"

// #include <bsoncxx/builder/basic/document.hpp>
// #include <bsoncxx/json.hpp>
// #include <cpr/cpr.h>
// #include "crow.h"
// #include <mongocxx/client.hpp>
// #include <mongocxx/instance.hpp>
// #include <mongocxx/uri.hpp>
// #include <mongocxx/exception/exception.hpp>
// #include <mongocxx/exception/bulk_write_exception.hpp>

// #include <chrono>
// #include <ctime>
// #include <sstream>
// #include <vector>
// #include <string>
// #include <iostream>

// auto ApiHandler::perform_realtime_update_complaints_analytics_from_reddit(const crow::request&
// req, std::shared_ptr<Database> db, std::shared_ptr<Reddit> reddit, const std::string& subreddit,
// const std::string& collection_name) -> crow::response {
//     try {
//         auto utc_ts_now = get_utc_timestamp_now();
//         auto utc_ts_one_day_ago = get_utc_timestamp_one_day_ago();
//         auto posts = reddit->get_posts(subreddit, 10, utc_ts_one_day_ago, utc_ts_now);
//         auto complaints = reddit->get_complaints_from_posts(posts);

//         std::vector<bsoncxx::document::value> complaint_docs;
//         for (auto &complaint: complaints) {
//             auto complaint_doc = bsoncxx::from_json(complaint.dump());
//             complaint_docs.push_back(complaint_doc);
//         }

//         int success_count = 0;
//         int failure_count = 0;

//         for (auto &complaint_doc : complaint_docs) {
//             try {
//                 auto result = db->insert_one(collection_name, complaint_doc);
//                 if (result) {
//                     std::cout << "Successfully inserted document." << std::endl;
//                     success_count++;
//                 } else {
//                     std::cerr << "Failed to insert document (no result returned)." << std::endl;
//                     failure_count++;
//                 }
//             } catch (const mongocxx::bulk_write_exception &e) {
//                 std::cerr << "Bulk write exception: " << e.what() << std::endl;
//                 failure_count++;
//             } catch (const mongocxx::exception &e) {
//                 if (std::string(e.what()).find("E11000") != std::string::npos) {
//                     std::cerr << "Duplicate key error - skipping this document." << std::endl;
//                 } else {
//                     std::cerr << "Insert failed: " << e.what() << std::endl;
//                     failure_count++;
//                 }
//             } catch (const std::exception &e) {
//                 std::cerr << "Unexpected error: " << e.what() << std::endl;
//                 failure_count++;
//             }
//         }

//         crow::json::wvalue response_data;
//         response_data["successful_insertion_count"] = success_count;
//         response_data["failed_insertion_count"] = failure_count;
//         return make_success_response(200, response_data, "Success");
//     }
//     catch (const std::exception& e) {
//         return make_error_response(500, std::string("Server error: ") + e.what());
//     }
// }