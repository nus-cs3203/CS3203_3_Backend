#include "api_handler.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <chrono>
#include <iostream>
#include <vector>

auto ApiHandler::get_sentiment_analytics_by_source(const crow::request& req, Database& db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"start_date", "end_date"})) {
            return make_error_response(400, "Invalid request format");
        }
        
        auto collection_name = "posts";
        auto start_date_utc_unix_timestamp_second = body["start_date"].i() * 1000;
        bsoncxx::types::b_date start_date{std::chrono::milliseconds(start_date_utc_unix_timestamp_second)};
        auto end_date_utc_unix_timestamp_second = body["end_date"].i() * 1000;
        bsoncxx::types::b_date end_date{std::chrono::milliseconds(end_date_utc_unix_timestamp_second)};

        mongocxx::pipeline pipeline{};

        pipeline.match(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("date", bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$gte", start_date),
                bsoncxx::builder::basic::kvp("$lte", end_date)
            ))
        ));

        pipeline.group(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id", "$source"), 
            bsoncxx::builder::basic::kvp("count", bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$sum", 1) 
            )),
            bsoncxx::builder::basic::kvp("sentiment_sum", bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$sum", "$sentiment") 
            ))
        ));

        auto cursor = db.aggregate(collection_name, pipeline);
        
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }

        crow::json::wvalue response_data;
        return make_success_response(200, response_data, "Posts retrieved");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::get_sentiment_analytics_by_category(const crow::request& req, Database& db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"start_date", "end_date"})) {
            return make_error_response(400, "Invalid request format");
        }
        
        auto collection_name = "posts";
        auto start_date_utc_unix_timestamp_second = body["start_date"].i() * 1000;
        bsoncxx::types::b_date start_date{std::chrono::milliseconds(start_date_utc_unix_timestamp_second)};
        auto end_date_utc_unix_timestamp_second = body["end_date"].i() * 1000;
        bsoncxx::types::b_date end_date{std::chrono::milliseconds(end_date_utc_unix_timestamp_second)};

        mongocxx::pipeline pipeline{};

        pipeline.match(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("date", bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$gte", start_date),
                bsoncxx::builder::basic::kvp("$lte", end_date)
            ))
        ));

        pipeline.group(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id", "$category"), 
            bsoncxx::builder::basic::kvp("count", bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$sum", 1) 
            )),
            bsoncxx::builder::basic::kvp("sentiment_sum", bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("$sum", "$sentiment") 
            ))
        ));

        auto cursor = db.aggregate(collection_name, pipeline);
        
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }

        crow::json::wvalue response_data;
        return make_success_response(200, response_data, "Posts retrieved");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::get_most_positive_posts(const crow::request& req, Database& db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"limit"})) {
            return make_error_response(400, "Invalid request format");
        }
        
        auto collection_name = "posts";
        auto limit = body["limit"].i();

        auto cursor = _get_posts_sorted_by(db, {"post.sentiment"}, {false}, limit);

        std::vector<crow::json::wvalue> documents;
        for (auto&& document: cursor) {
            auto document_json = bsoncxx::to_json(document);
            documents.push_back(crow::json::load(document_json));
        }

        crow::json::wvalue response_data;
        response_data["posts"] = std::move(documents);
        return make_success_response(200, response_data, "Posts retrieved");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::get_most_negative_posts(const crow::request& req, Database& db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"limit"})) {
            return make_error_response(400, "Invalid request format");
        }
        
        auto collection_name = "posts";
        auto limit = body["limit"].i();

        auto cursor = _get_posts_sorted_by(db, {"post.sentiment"}, {true}, limit);

        std::vector<crow::json::wvalue> documents;
        for (auto&& document: cursor) {
            auto document_json = bsoncxx::to_json(document);
            documents.push_back(crow::json::load(document_json));
        }

        crow::json::wvalue response_data;
        response_data["posts"] = std::move(documents);
        return make_success_response(200, response_data, "Posts retrieved");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::_get_posts_sorted_by(Database& db, const std::vector<std::string>& keys, const std::vector<bool>& ascending_orders, const int& limit) -> mongocxx::cursor {
    std::string collection_name = "posts";
    bsoncxx::builder::basic::document sort_builder{};
    for (int i = 0; i < keys.size(); ++i) {
        std::string key = keys[i];
        int direction = ascending_orders[i] ? 1 : -1;
        sort_builder.append(bsoncxx::builder::basic::kvp(key, direction));
    }
    mongocxx::options::find option;
    option.sort(sort_builder.view());
    option.limit(limit);

    auto cursor = db.find(collection_name, {}, option);
    return cursor;
}