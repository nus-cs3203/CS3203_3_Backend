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

auto ApiHandler::get_posts_grouped_by_field(const crow::request& req, Database& db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"start_date", "end_date", "group_by_field"})) {
            return make_error_response(400, "Invalid request format");
        }
        
        auto start_date_str = body["start_date"].s();
        auto start_date_ts = string_to_utc_unix_timestamp(start_date_str, Constants::DATETIME_FORMAT) * 1000;
        bsoncxx::types::b_date start_date{std::chrono::milliseconds(start_date_ts)};

        auto end_date_str = body["end_date"].s();
        auto end_date_ts = string_to_utc_unix_timestamp(end_date_str, Constants::DATETIME_FORMAT) * 1000;
        bsoncxx::types::b_date end_date{std::chrono::milliseconds(end_date_ts)};

        auto group_by_field = body["group_by_field"].s();

        bsoncxx::document::value filter = make_document(
            kvp("date", make_document(
                kvp("$gte", start_date),
                kvp("$lte", end_date)
            ))
        );

        auto cursor = _get_posts_grouped_by_field(db, group_by_field, filter);

        crow::json::wvalue result;
        for (auto&& document: cursor) {
            auto document_json = bsoncxx::to_json(document);
            crow::json::rvalue rval_json = crow::json::load(document_json);

            crow::json::wvalue sub_result;
            sub_result["count"] = rval_json["count"];
            sub_result["avg_sentiment"] = rval_json["avg_sentiment"];
            result[rval_json["_id"].s()] = std::move(sub_result);
        }

        crow::json::wvalue response_data;
        response_data["result"] = std::move(result);
        return make_success_response(200, response_data, "Analytics result retrieved.");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::get_posts_grouped_by_field_over_time(const crow::request& req, Database& db) -> crow::response {
    try
    {
        auto body = crow::json::load(req.body);
        if (!validate_request(body, {"start_date", "end_date", "group_by_field", "time_bucket_regex"})) {
            return make_error_response(400, "Invalid request format");
        }

        auto start_date_str = body["start_date"].s();
        auto start_date_ts = string_to_utc_unix_timestamp(start_date_str, Constants::DATETIME_FORMAT) * 1000;
        bsoncxx::types::b_date start_date{std::chrono::milliseconds(start_date_ts)};

        auto end_date_str = body["end_date"].s();
        auto end_date_ts = string_to_utc_unix_timestamp(end_date_str, Constants::DATETIME_FORMAT) * 1000;
        bsoncxx::types::b_date end_date{std::chrono::milliseconds(end_date_ts)};

        auto group_by_field = body["group_by_field"].s();

        auto time_bucket_regex = body["time_bucket_regex"].s();

        bsoncxx::document::value filter = make_document(
            kvp("date", make_document(
                kvp("$gte", start_date),
                kvp("$lte", end_date)
            ))
        );

        auto cursor = _get_posts_grouped_by_field_over_time(db, group_by_field, time_bucket_regex, filter);

        crow::json::wvalue result;
        for (auto&& document: cursor) {
            auto document_json = bsoncxx::to_json(document);
            crow::json::rvalue rval_json = crow::json::load(document_json);

            auto time_bucket_value = rval_json["_id"]["time_bucket"].s();
            auto group_by_field_value = rval_json["_id"][group_by_field].s();

            result[time_bucket_value][group_by_field_value]["count"] = rval_json["count"].i();
            result[time_bucket_value][group_by_field_value]["avg_sentiment"] = rval_json["avg_sentiment"].d();
        }

        crow::json::wvalue response_data;
        response_data["result"] = std::move(result);
        return make_success_response(200, response_data, "Analytics result retrieved.");
    }
    catch (const std::exception& e)
    {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::get_posts_grouped_by_sentiment_value(const crow::request& req, Database& db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"start_date", "end_date", "bucket_size"})) {
            return make_error_response(400, "Invalid request format");
        }

        auto start_date_str = body["start_date"].s();
        auto start_date_ts = string_to_utc_unix_timestamp(start_date_str, Constants::DATETIME_FORMAT) * 1000;
        bsoncxx::types::b_date start_date{std::chrono::milliseconds(start_date_ts)};

        auto end_date_str = body["end_date"].s();
        auto end_date_ts = string_to_utc_unix_timestamp(end_date_str, Constants::DATETIME_FORMAT) * 1000;
        bsoncxx::types::b_date end_date{std::chrono::milliseconds(end_date_ts)};

        double bucket_size = body["bucket_size"].d();

        if (bucket_size <= 0) {
            return make_error_response(400, "Bucket size must be greater than 0");
        }

        bsoncxx::document::value filter = make_document(
            kvp("date", make_document(
                kvp("$gte", start_date),
                kvp("$lte", end_date)
            ))
        );

        auto cursor = _get_posts_grouped_by_sentiment_value(db, bucket_size, filter);

        std::vector<crow::json::wvalue> result;
        for (auto&& doc : cursor) {
            auto doc_json = bsoncxx::to_json(doc);
            crow::json::rvalue rval_json = crow::json::load(doc_json);

            crow::json::wvalue sub_result;
            sub_result["left_bound_inclusive"] = rval_json["_id"];
            sub_result["right_bound_exclusive"] = rval_json["_id"].d() + bucket_size;
            sub_result["count"] = rval_json["count"];
            result.push_back(sub_result);
        }

        crow::json::wvalue response_data;
        response_data["result"] = std::move(result);
        return make_success_response(200, response_data, "Analytics result retrieved.");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}


auto ApiHandler::get_posts_sorted_by_fields(const crow::request& req, Database& db) -> crow::response  {
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

        auto cursor = _get_posts_sorted_by_fields(db, keys, ascending_orders, limit);

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


auto ApiHandler::_get_posts_grouped_by_field(Database& db, const std::string& group_by_field, const bsoncxx::document::view& filter) -> mongocxx::cursor {
    mongocxx::pipeline pipeline{};

    pipeline.match(filter);

    pipeline.group(make_document(
        kvp("_id", "$" + group_by_field),
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

    auto cursor = db.aggregate(Constants::COLLECTION_POSTS, pipeline);
    return cursor;
}

auto ApiHandler::_get_posts_grouped_by_field_over_time(
    Database& db, 
    const std::string& group_by_field, 
    const std::string& time_bucket_regex, 
    const bsoncxx::document::view& filter
) -> mongocxx::cursor {

    mongocxx::pipeline pipeline{};

    pipeline.match(filter);

    pipeline.group(
        make_document(
            kvp("_id",
                make_document(
                    kvp(
                        "time_bucket",
                        make_document(
                            kvp("$dateToString",
                                make_document(
                                    kvp("format", time_bucket_regex),
                                    kvp("date", "$date")
                                )
                            )
                        )
                    ),
                    kvp(group_by_field, "$" + group_by_field)
                )
            ),
            kvp("count", make_document(kvp("$sum", 1))),
            kvp("avg_sentiment", make_document(kvp("$avg", "$sentiment")))
        )
    );

    pipeline.sort(
        make_document(
            kvp("time_bucket", 1)
        )
    );

    auto cursor = db.aggregate("posts", pipeline);
    return cursor;
}

auto ApiHandler::_get_posts_grouped_by_sentiment_value(Database& db, const double &bucket_size, const bsoncxx::document::view& filter) -> mongocxx::cursor {
    mongocxx::pipeline pipeline{};

    pipeline.match(filter);

    double min_sentiment = -1.0;
    double max_sentiment = 1.01;
    
    std::vector<double> boundaries;
    for (double val = min_sentiment; val < max_sentiment; val += bucket_size) {
        boundaries.push_back(val);
    }   
    boundaries.push_back(max_sentiment);

    bsoncxx::builder::basic::array boundaries_array;
    for (auto b: boundaries) {
        boundaries_array.append(b);
    }

    auto bucket_stage = make_document(
        kvp("groupBy", std::string{"$sentiment"}),
        kvp("boundaries", boundaries_array.extract()),
        kvp("default", std::string{"OutOfRange"}),
        kvp("output", make_document(
            kvp("count", make_document(kvp("$sum", 1)))
        ))
    );

    pipeline.bucket(bucket_stage.view());

    auto cursor = db.aggregate(Constants::COLLECTION_POSTS, pipeline);
    return cursor;
}



auto ApiHandler::_get_posts_sorted_by_fields(Database& db, const std::vector<std::string>& keys, const std::vector<bool>& ascending_orders, const int& limit) -> mongocxx::cursor {
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