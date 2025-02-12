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

auto ApiHandler::get_sentiment_analytics_by_category_over_time(const crow::request& req, Database& db) -> crow::response
{
    try
    {
        auto body = crow::json::load(req.body);
        if (!validate_request(body, {"start_date", "end_date", "time_granularity_regex"})) {
            return make_error_response(400, "Invalid request format");
        }
        auto start_date_utc_unix_ms = static_cast<int64_t>(body["start_date"].i()) * 1000;
        auto end_date_utc_unix_ms   = static_cast<int64_t>(body["end_date"].i()) * 1000;

        bsoncxx::types::b_date start_date{std::chrono::milliseconds(start_date_utc_unix_ms)};
        bsoncxx::types::b_date end_date{std::chrono::milliseconds(end_date_utc_unix_ms)};
        auto time_granularity_regex = body["time_granularity_regex"].s();

        mongocxx::pipeline pipeline{};

        pipeline.match(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("date",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$gte", start_date),
                    bsoncxx::builder::basic::kvp("$lte", end_date)
                )
            )
        ));

        pipeline.group(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp(
                "_id",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp(
                        "month",
                        bsoncxx::builder::basic::make_document(
                            bsoncxx::builder::basic::kvp(
                                "$dateToString",
                                bsoncxx::builder::basic::make_document(
                                    bsoncxx::builder::basic::kvp("format", time_granularity_regex),
                                    bsoncxx::builder::basic::kvp("date", "$date")
                                )
                            )
                        )
                    ),
                    bsoncxx::builder::basic::kvp("category", "$category")
                )
            ),
            bsoncxx::builder::basic::kvp("count",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$sum", 1)
                )
            ),
            bsoncxx::builder::basic::kvp("avg_sentiment",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$avg", "$sentiment")
                )
            )
        ));

        pipeline.sort(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id.month", 1)
        ));

        auto cursor = db.aggregate("posts", pipeline);

        std::vector<crow::json::wvalue> documents;
        for (auto&& document: cursor) {
            auto document_json = bsoncxx::to_json(document);
            documents.push_back(crow::json::load(document_json));
        }

        crow::json::wvalue response_data;
        response_data["result"] = std::move(documents);
        return make_success_response(200, response_data, "Analytics (category across time) retrieved.");
    }
    catch (const std::exception& e)
    {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::get_sentiment_analytics_by_source_over_time(const crow::request& req, Database& db) -> crow::response
{
    try
    {
        auto body = crow::json::load(req.body);
        if (!validate_request(body, {"start_date", "end_date", "time_granularity_regex"})) {
            return make_error_response(400, "Invalid request format");
        }
        auto start_date_utc_unix_ms = static_cast<int64_t>(body["start_date"].i()) * 1000;
        auto end_date_utc_unix_ms   = static_cast<int64_t>(body["end_date"].i()) * 1000;

        bsoncxx::types::b_date start_date{std::chrono::milliseconds(start_date_utc_unix_ms)};
        bsoncxx::types::b_date end_date{std::chrono::milliseconds(end_date_utc_unix_ms)};
        auto time_granularity_regex = body["time_granularity_regex"].s();

        mongocxx::pipeline pipeline{};

        pipeline.match(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("date",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$gte", start_date),
                    bsoncxx::builder::basic::kvp("$lte", end_date)
                )
            )
        ));

        pipeline.group(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp(
                "_id",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp(
                        "month",
                        bsoncxx::builder::basic::make_document(
                            bsoncxx::builder::basic::kvp(
                                "$dateToString",
                                bsoncxx::builder::basic::make_document(
                                    bsoncxx::builder::basic::kvp("format", time_granularity_regex),
                                    bsoncxx::builder::basic::kvp("date", "$date")
                                )
                            )
                        )
                    ),
                    bsoncxx::builder::basic::kvp("source", "$source")
                )
            ),
            bsoncxx::builder::basic::kvp("count",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$sum", 1)
                )
            ),
            bsoncxx::builder::basic::kvp("avg_sentiment",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$avg", "$sentiment")
                )
            )
        ));

        pipeline.sort(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id.month", 1)
        ));

        auto cursor = db.aggregate("posts", pipeline);

        std::vector<crow::json::wvalue> documents;
        for (auto&& document: cursor) {
            auto document_json = bsoncxx::to_json(document);
            documents.push_back(crow::json::load(document_json));
        }

        crow::json::wvalue response_data;
        response_data["result"] = std::move(documents);
        return make_success_response(200, response_data, "Analytics (category across time) retrieved.");
    }
    catch (const std::exception& e)
    {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::get_sentiment_analytics_by_value(const crow::request& req, Database& db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"start_date", "end_date", "bucket_size"})) {
            return make_error_response(400, "Invalid request format");
        }

        auto start_date_utc_unix_ms = static_cast<long long>(body["start_date"].i()) * 1000; // to ms
        auto end_date_utc_unix_ms   = static_cast<long long>(body["end_date"].i())   * 1000; // to ms
        bsoncxx::types::b_date start_date{std::chrono::milliseconds(start_date_utc_unix_ms)};
        bsoncxx::types::b_date end_date{std::chrono::milliseconds(end_date_utc_unix_ms)};

        double bucket_size = body["bucket_size"].d();
        if (bucket_size <= 0) {
            return make_error_response(400, "Bucket size must be greater than 0");
        }

        double min_sentiment = -1.0;
        double max_sentiment = 1.0;
        
        std::vector<double> boundaries;
        for (double val = min_sentiment; val < max_sentiment; val += bucket_size) {
            boundaries.push_back(val);
        }   
        boundaries.push_back(max_sentiment);

        mongocxx::pipeline pipeline{};

        pipeline.match(
            bsoncxx::builder::basic::make_document(
                bsoncxx::builder::basic::kvp("date", 
                    bsoncxx::builder::basic::make_document(
                        bsoncxx::builder::basic::kvp("$gte", start_date),
                        bsoncxx::builder::basic::kvp("$lte", end_date)
                    )
                )
            )
        );

        {
            using bsoncxx::builder::basic::array;
            using bsoncxx::builder::basic::kvp;
            using bsoncxx::builder::basic::make_document;

            bsoncxx::builder::basic::array boundaries_array;
            for (auto b : boundaries) {
                boundaries_array.append(b);
            }

            auto bucket_stage = make_document(
                kvp("groupBy", std::string{"$sentiment"}),
                kvp("boundaries", boundaries_array.extract()),
                kvp("default", std::string{"OutOfRange"}),
                kvp("output", make_document(
                    kvp("count", make_document(kvp("$sum", 1))),
                    kvp("avg_sentiment", make_document(kvp("$avg", std::string{"$sentiment"})))
                ))
            );

            pipeline.bucket(bucket_stage.view());
        }

        auto collection_name = "posts"; 
        auto cursor = db.aggregate(collection_name, pipeline);

        std::vector<crow::json::wvalue> documents;
        for (auto&& doc : cursor) {
            auto doc_json = bsoncxx::to_json(doc);
            documents.push_back(crow::json::load(doc_json));
        }

        crow::json::wvalue response_data;
        response_data["result"] = std::move(documents);

        return make_success_response(200, response_data, "Sentiment bucket analytics retrieved.");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

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
            bsoncxx::builder::basic::kvp("count",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$sum", 1)
                )
            ),
            bsoncxx::builder::basic::kvp("avg_sentiment",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$avg", "$sentiment")
                )
            )
        ));

        auto cursor = db.aggregate(collection_name, pipeline);

        std::vector<crow::json::wvalue> documents;
        for (auto&& document: cursor) {
            auto document_json = bsoncxx::to_json(document);
            documents.push_back(crow::json::load(document_json));
        }

        crow::json::wvalue response_data;
        response_data["result"] = std::move(documents);
        return make_success_response(200, response_data, "Analytics result retrieved.");
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
            bsoncxx::builder::basic::kvp("count",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$sum", 1)
                )
            ),
            bsoncxx::builder::basic::kvp("avg_sentiment",
                bsoncxx::builder::basic::make_document(
                    bsoncxx::builder::basic::kvp("$avg", "$sentiment")
                )
            )
        ));

        auto cursor = db.aggregate(collection_name, pipeline);

        std::vector<crow::json::wvalue> documents;
        for (auto&& document: cursor) {
            auto document_json = bsoncxx::to_json(document);
            documents.push_back(crow::json::load(document_json));
        }

        crow::json::wvalue response_data;
        response_data["result"] = std::move(documents);
        return make_success_response(200, response_data, "Analytics result retrieved.");
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