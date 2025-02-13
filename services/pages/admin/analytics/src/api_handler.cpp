#include "api_handler.hpp"
#include "constants.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <chrono>
#include <iostream>
#include <vector>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

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

        pipeline.match(make_document(
            kvp("date",
                make_document(
                    kvp("$gte", start_date),
                    kvp("$lte", end_date)
                )
            )
        ));

        pipeline.group(make_document(
            kvp(
                "_id",
                make_document(
                    kvp(
                        "month",
                        make_document(
                            kvp(
                                "$dateToString",
                                make_document(
                                    kvp("format", time_granularity_regex),
                                    kvp("date", "$date")
                                )
                            )
                        )
                    ),
                    kvp("category", "$category")
                )
            ),
            kvp("count",
                make_document(
                    kvp("$sum", 1)
                )
            ),
            kvp("avg_sentiment",
                make_document(
                    kvp("$avg", "$sentiment")
                )
            )
        ));

        pipeline.sort(make_document(
            kvp("_id.month", 1)
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

        pipeline.match(make_document(
            kvp("date",
                make_document(
                    kvp("$gte", start_date),
                    kvp("$lte", end_date)
                )
            )
        ));

        pipeline.group(make_document(
            kvp(
                "_id",
                make_document(
                    kvp(
                        "month",
                        make_document(
                            kvp(
                                "$dateToString",
                                make_document(
                                    kvp("format", time_granularity_regex),
                                    kvp("date", "$date")
                                )
                            )
                        )
                    ),
                    kvp("source", "$source")
                )
            ),
            kvp("count",
                make_document(
                    kvp("$sum", 1)
                )
            ),
            kvp("avg_sentiment",
                make_document(
                    kvp("$avg", "$sentiment")
                )
            )
        ));

        pipeline.sort(make_document(
            kvp("_id.month", 1)
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
            make_document(
                kvp("date", 
                    make_document(
                        kvp("$gte", start_date),
                        kvp("$lte", end_date)
                    )
                )
            )
        );

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

        pipeline.match(make_document(
            kvp("date", make_document(
                kvp("$gte", start_date),
                kvp("$lte", end_date)
            ))
        ));

        pipeline.group(make_document(
            kvp("_id", "$source"),
            kvp("count",
                make_document(
                    kvp("$sum", 1)
                )
            ),
            kvp("avg_sentiment",
                make_document(
                    kvp("$avg", "$sentiment")
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

        pipeline.match(make_document(
            kvp("date", make_document(
                kvp("$gte", start_date),
                kvp("$lte", end_date)
            ))
        ));

        pipeline.group(make_document(
            kvp("_id", "$category"),
            kvp("count",
                make_document(
                    kvp("$sum", 1)
                )
            ),
            kvp("avg_sentiment",
                make_document(
                    kvp("$avg", "$sentiment")
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

auto ApiHandler::get_posts_sorted(const crow::request& req, Database& db) -> crow::response  {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"keys", "ascending_orders", "limit"})) {
            return make_error_response(400, "Invalid request format");
        }
        if (body["keys"].t() != crow::json::type::List) {
            return make_error_response(400, "Invalid format: 'keys' must be an array");
        }
        if (body["ascending_orders"].t() != crow::json::type::List) {
            return make_error_response(400, "Invalid format: 'ascending_orders' must be an array");
        }
        
        auto collection_name = "posts";
        std::vector<std::string> keys;
        auto keys_json = body["keys"];
        for (const auto& key: keys_json.lo()) {
            keys.push_back("posts." + static_cast<std::string>(key.s())); 
        }
        std::vector<bool> ascending_orders;
        auto ascending_orders_json = body["ascending_orders"];
        for (const auto& ascending_order: ascending_orders_json.lo()) {
            ascending_orders.push_back(ascending_order.b());
        }
        auto limit = body["limit"].i();

        auto cursor = _get_posts_sorted(db, keys, ascending_orders, limit);

        std::vector<crow::json::wvalue> documents;
        for (auto&& document: cursor) {
            auto document_json = bsoncxx::to_json(document);
            crow::json::rvalue rval_json = crow::json::load(document_json);
            crow::json::wvalue wval_json = crow::json::load(document_json);
            wval_json["date"] = utc_unix_timestamp_to_string(rval_json["date"]["$date"].i() / 1000, Constants::DATETIME_FORMAT);
            documents.push_back(std::move(wval_json));
        }

        crow::json::wvalue response_data;
        response_data["posts"] = std::move(documents);
        return make_success_response(200, response_data, "Post(s) retrieved.");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::_get_posts_sorted(Database& db, const std::vector<std::string>& keys, const std::vector<bool>& ascending_orders, const int& limit) -> mongocxx::cursor {
    if (keys.size() != ascending_orders.size()) {
        throw std::invalid_argument("keys and ascending_orders vectors must have the same size.");
    }

    mongocxx::options::find option;

    bsoncxx::builder::basic::document sort_builder{};
    for (int i = 0; i < keys.size(); ++i) {
        std::string key = keys[i];
        int direction = ascending_orders[i] ? 1 : -1;
        sort_builder.append(kvp(key, direction));
    }
    option.sort(sort_builder.view());

    option.limit(limit);

    auto cursor = db.find(Constants::COLLECTION_POSTS, {}, option);
    return cursor;
}