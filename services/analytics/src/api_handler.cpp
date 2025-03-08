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
#include <map>
#include <set>
#include <unordered_set>
#include <vector>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto ApiHandler::get_category_analytics_by_name(const crow::request& req, std::shared_ptr<Database> db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"name"})) {
            return make_error_response(400, "Invalid request format");
        }
        auto name = body["name"].s();

        bsoncxx::document::value filter = make_document(
            kvp("name", name)
        );

        auto result = db->find_one(Constants::COLLECTION_CATEGORY_ANALYTICS, filter);

        crow::json::wvalue response_data;
        response_data["document"] = {};

        if (!result.has_value()) {
            return make_success_response(200, response_data, "No matching category_analytics found");
        }

        auto document_json = bsoncxx::to_json(result.value());
        auto document_rvalue = crow::json::load(document_json);

        response_data["document"] = document_rvalue;
        return make_success_response(200, response_data, "Retrieved analytics successfully");
    
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::get_complaints_statistics(const crow::request& req, std::shared_ptr<Database> db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        crow::json::rvalue filter_json = create_empty_crow_json_rvalue();
        if (body.has("filter")) {
            filter_json = body["filter"];
        }
        auto filter_bson = _create_filter_complaints(filter_json); 

        auto pipeline = _create_aggregate_pipeline_complaints_statistics(filter_bson.view());
        auto cursor = db->aggregate(Constants::COLLECTION_COMPLAINTS, pipeline);
        auto result = _read_cursor_complaints_statistics(cursor);
        
        crow::json::wvalue response_data;
        response_data["result"] = std::move(result);
        return make_success_response(200, response_data, "Analytics result retrieved.");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::_create_filter_complaints(const crow::json::rvalue& json) -> bsoncxx::document::value {
    bsoncxx::builder::basic::document filter_builder{};

    if (json.has("keyword")) {
        auto keyword = json["keyword"].s();
        filter_builder.append(
            kvp("$text", 
                make_document(
                    kvp("$search", static_cast<std::string>(keyword))
                )
            )
        );
    }

    if (json.has("category")) {
        auto category = json["category"].s();
        filter_builder.append(kvp("category", static_cast<std::string>(category)));
    }

    if (json.has("source")) {
        auto source = json["source"].s();
        filter_builder.append(kvp("source", static_cast<std::string>(source)));
    }

    if (json.has("start_date")) {
        auto start_date = json_date_to_bson_date(json["start_date"]);
        filter_builder.append(
            kvp("date", 
                make_document(
                    kvp("$gte", start_date)
                )
            )
        );
    }

    if (json.has("end_date")) {
        auto end_date = json_date_to_bson_date(json["end_date"]);
        filter_builder.append(
            kvp("date", 
                make_document(
                    kvp("$lte", end_date)
                )
            )
        );
    }

    if (json.has("min_sentiment")) {
        auto min_sentiment = json["min_sentiment"].d();
        filter_builder.append(
            kvp("sentiment", 
                make_document(
                    kvp("$gte", min_sentiment)
                )
            )
        );
    }

    if (json.has("max_sentiment")) {
        auto max_sentiment = json["max_sentiment"].d();
        filter_builder.append(
            kvp("sentiment", 
                make_document(
                    kvp("$lte", max_sentiment)
                )
            )
        );
    }
    
    return filter_builder.extract();
}

auto ApiHandler::_create_aggregate_pipeline_complaints_statistics(const bsoncxx::document::view& filter) -> mongocxx::pipeline {
    mongocxx::pipeline pipeline{};

    pipeline.match(filter);

    pipeline.group(
        make_document(
            kvp("_id", bsoncxx::types::b_null()),
            kvp("count", make_document(kvp("$sum", 1))),
            kvp("avg_sentiment", make_document(kvp("$avg", "$sentiment")))
        )
    );

    return pipeline;
}

auto ApiHandler::_read_cursor_complaints_statistics(mongocxx::cursor& cursor) -> crow::json::wvalue {
    crow::json::wvalue result;
    result["count"] = 0;
    result["avg_sentiment"] = 0;
    for (auto&& document: cursor) {
        auto document_json = bsoncxx::to_json(document);
        crow::json::rvalue rval_json = crow::json::load(document_json);
        result["count"] = rval_json["count"];
        result["avg_sentiment"] = rval_json["avg_sentiment"];
    }
    return result;
}

auto ApiHandler::get_complaints_sorted_by_fields(const crow::request& req, std::shared_ptr<Database> db) -> crow::response  {
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
        
        std::vector<std::string> keys;
        auto keys_json = body["keys"];
        for (const auto& key: keys_json.lo()) {
            keys.push_back(static_cast<std::string>(key.s())); 
        }

        std::vector<bool> ascending_orders;
        auto ascending_orders_json = body["ascending_orders"];
        for (const auto& ascending_order: ascending_orders_json.lo()) {
            ascending_orders.push_back(ascending_order.b());
        }

        auto limit = body["limit"].i();

        crow::json::rvalue filter_json = create_empty_crow_json_rvalue();
        if (body.has("filter")) {
            filter_json = body["filter"];
        }
        auto filter_bson = _create_filter_complaints(filter_json); 

        bsoncxx::builder::basic::document sort_builder{};
        auto find_option = _create_find_option_complaints_sorted_by_fields(sort_builder, keys, ascending_orders, limit);
        auto cursor = db->find(Constants::COLLECTION_COMPLAINTS, filter_bson.view(), find_option);
        auto documents = _read_cursor_complaints_sorted_by_fields(cursor);

        crow::json::wvalue response_data;
        response_data[Constants::COLLECTION_COMPLAINTS] = std::move(documents);
        return make_success_response(200, response_data, "Complaint(s) retrieved.");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::_create_find_option_complaints_sorted_by_fields(bsoncxx::builder::basic::document& sort_builder, const std::vector<std::string>& keys, const std::vector<bool>& ascending_orders, const int& limit) -> mongocxx::options::find {
    if (keys.size() != ascending_orders.size()) {
        throw std::invalid_argument("keys and ascending_orders vectors must have the same size.");
    }

    mongocxx::options::find option;

    for (int i = 0; i < keys.size(); ++i) {
        std::string key = keys[i];
        int direction = ascending_orders[i] ? 1 : -1;
        sort_builder.append(kvp(key, direction));
    }
    option.sort(sort_builder.view());

    option.limit(limit);

    return option;
}

auto ApiHandler::_read_cursor_complaints_sorted_by_fields(mongocxx::cursor& cursor) -> std::vector<crow::json::wvalue> {
    std::vector<crow::json::wvalue> documents;
    for (auto&& document: cursor) {
        auto document_json = bsoncxx::to_json(document);
        crow::json::rvalue rval_json = crow::json::load(document_json);
        crow::json::wvalue wval_json = crow::json::load(document_json);
        wval_json["date"] = utc_unix_timestamp_to_string(rval_json["date"]["$date"].i() / 1000, Constants::DATETIME_FORMAT);
        documents.push_back(std::move(wval_json));
    }
    return documents;
}

auto ApiHandler::get_complaints_statistics_over_time(const crow::request& req, std::shared_ptr<Database> db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"start_date", "end_date"})) {
            return make_error_response(400, "Invalid request format");
        }

        auto start_date = static_cast<std::string>(body["start_date"].s()); // dd-mm-YYYY HH:MM:SS
        int start_month = extract_month_from_timestamp_str(start_date);
        int start_year = extract_year_from_timestamp_str(start_date);

        auto end_date = static_cast<std::string>(body["end_date"].s()); // dd-mm-YYYY HH:MM:SS
        int end_month = extract_month_from_timestamp_str(end_date);
        int end_year = extract_year_from_timestamp_str(end_date);

        crow::json::rvalue filter_json = create_empty_crow_json_rvalue();
        if (body.has("filter")) {
            filter_json = body["filter"];
        }
        auto filter_bson = _create_filter_complaints(filter_json); 

        auto pipeline = _create_aggregate_pipeline_complaints_statistics_over_time(filter_bson.view());
        auto cursor = db->aggregate(Constants::COLLECTION_COMPLAINTS, pipeline);
        auto documents = _read_cursor_complaints_statistics_over_time(cursor);

        auto month_range = _create_month_range(start_month, start_year, end_month, end_year);
        documents = _format_documents_complaints_statistics_over_time(documents, month_range);

        crow::json::wvalue response_data;
        response_data["result"] = std::move(documents);
        return make_success_response(200, response_data, "Analytics result retrieved.");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::_create_aggregate_pipeline_complaints_statistics_over_time(const bsoncxx::document::view& filter) -> mongocxx::pipeline {
    mongocxx::pipeline pipeline{};

    pipeline.match(filter);

    pipeline.group(make_document(
        kvp("_id", make_document(
            kvp("year", make_document(kvp("$year", "$date"))),
            kvp("month", make_document(kvp("$month", "$date")))
        )),
        kvp("count", make_document(kvp("$sum", 1))),
        kvp("avg_sentiment", make_document(kvp("$avg", "$sentiment")))
    ));

    return pipeline;
}

auto ApiHandler::_read_cursor_complaints_statistics_over_time(mongocxx::cursor& cursor) -> std::vector<crow::json::wvalue> {
    std::vector<crow::json::wvalue> documents;
    for (auto&& document: cursor) {
        auto document_json = bsoncxx::to_json(document);
        crow::json::wvalue wval_json = crow::json::load(document_json);
        documents.push_back(std::move(wval_json));
    }
    return documents;
}

auto ApiHandler::_create_month_range(const int& start_month, const int& start_year, const int& end_month, const int& end_year) -> std::vector<std::pair<int, int>> {
    std::vector<std::pair<int, int>> result;
    if (start_year > end_year) {
        return result;
    }

    if (start_year == end_year) {
        for (int month = start_month; month <= end_month; ++month) {
            result.push_back({month, start_year});
        }
        return result;
    }

    for (int month = start_month; month <= 12; ++month) {
        result.push_back({month, start_year});
    }
    for (int year = start_year + 1; year < end_year; ++year) {
        for (int month = 1; month <= 12; ++month) {
            result.push_back({month, year});
        }
    }
    for (int month = 1; month <= end_month; ++month) {
        result.push_back({month, end_year});
    }

    return result;
}

auto ApiHandler::_format_documents_complaints_statistics_over_time(std::vector<crow::json::wvalue>& documents, const std::vector<std::pair<int, int>>& month_range) -> std::vector<crow::json::wvalue> {
    struct Statistics {
        int count;
        double avg_sentiment;
    };
    
    std::map<std::pair<int, int>, Statistics> mapper;

    for (auto &doc_wval_json: documents) {
        crow::json::rvalue doc_rval_json = crow::json::load(doc_wval_json.dump());

        int month = doc_rval_json["_id"]["month"].i();
        int year = doc_rval_json["_id"]["year"].i();

        int count = doc_rval_json["count"].i();
        double avg_sentiment = doc_rval_json["avg_sentiment"].d();

        mapper[{month, year}] = Statistics{count, avg_sentiment};
    }

    std::vector<crow::json::wvalue> result;
    for (const auto &[month, year]: month_range) {
        Statistics stat;
        if (mapper.find({month, year}) != mapper.end()) {
            stat.count = mapper[{month, year}].count;
            stat.avg_sentiment = mapper[{month, year}].avg_sentiment;
        }

        crow::json::wvalue wval_json;
        wval_json["date"] = create_month_year_str(month, year);
        wval_json["data"]["count"] = stat.count;
        wval_json["data"]["avg_sentiment"] = stat.avg_sentiment;
        result.push_back(std::move(wval_json));
    }

    return result;
}


auto ApiHandler::get_complaints_grouped_by_field(const crow::request& req, std::shared_ptr<Database> db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"start_date", "end_date", "group_by_field"})) {
            return make_error_response(400, "Invalid request format");
        }
        auto start_date = json_date_to_bson_date(body["start_date"]);
        auto end_date = json_date_to_bson_date(body["end_date"]);
        
        auto group_by_field = body["group_by_field"].s();

        bsoncxx::document::value filter = make_document(
            kvp("date", make_document(
                kvp("$gte", start_date),
                kvp("$lte", end_date)
            ))
        );

        auto cursor = _get_complaints_grouped_by_field_get_cursor(db, group_by_field, filter);
        auto result = _get_complaints_grouped_by_field_read_cursor(cursor, _get_group_by_field_all_distinct_values(db, group_by_field));
        
        crow::json::wvalue response_data;
        response_data["result"] = std::move(result);
        return make_success_response(200, response_data, "Analytics result retrieved.");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::_get_complaints_grouped_by_field_get_cursor(std::shared_ptr<Database> db, const std::string& group_by_field, const bsoncxx::document::view& filter) -> mongocxx::cursor {
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

    auto cursor = db->aggregate(Constants::COLLECTION_COMPLAINTS, pipeline);
    return cursor;
}

auto ApiHandler::_get_complaints_grouped_by_field_read_cursor(mongocxx::cursor& cursor, const std::vector<std::string>& categories) -> crow::json::wvalue {
    std::unordered_set<std::string> added_categories;

    crow::json::wvalue result;
    for (auto&& document: cursor) {
        auto document_json = bsoncxx::to_json(document);
        crow::json::rvalue rval_json = crow::json::load(document_json);

        crow::json::wvalue sub_result;
        sub_result["count"] = rval_json["count"];
        sub_result["avg_sentiment"] = rval_json["avg_sentiment"];
        auto category = rval_json["_id"].s();
        result[category] = std::move(sub_result);
        added_categories.insert(category);
    }

    for (auto &category: categories) {
        if (added_categories.find(category) != added_categories.end()) {
            continue;
        }
        crow::json::wvalue sub_result;
        sub_result["count"] = 0;
        sub_result["avg_sentiment"] = 0;
        result[category] = std::move(sub_result);
        added_categories.insert(category);
    }

    return result;
}

auto ApiHandler::get_complaints_grouped_by_field_over_time(const crow::request& req, std::shared_ptr<Database> db) -> crow::response{
    try
    {
        auto body = crow::json::load(req.body);
        if (!validate_request(body, {"start_date", "end_date", "group_by_field"})) {
            return make_error_response(400, "Invalid request format");
        }

        auto start_date_str = body["start_date"].s();
        auto start_date_ts  = string_to_utc_unix_timestamp(start_date_str, Constants::DATETIME_FORMAT) * 1000;
        auto end_date_str   = body["end_date"].s();
        auto end_date_ts    = string_to_utc_unix_timestamp(end_date_str, Constants::DATETIME_FORMAT) * 1000;

        bsoncxx::types::b_date start_date{std::chrono::milliseconds(start_date_ts)};
        bsoncxx::types::b_date end_date{std::chrono::milliseconds(end_date_ts)};

        std::string group_by_field = body["group_by_field"].s();  // Typically "category"

        bsoncxx::document::value filter = make_document(
            kvp("date", make_document(
                kvp("$gte", start_date),
                kvp("$lte", end_date)
            ))
        );

        auto cursor = _get_complaints_grouped_by_field_over_time_get_cursor(db, group_by_field, filter);
        auto resultArray = _get_complaints_grouped_by_field_over_time_read_cursor(cursor, _get_group_by_field_all_distinct_values(db, group_by_field), group_by_field, start_date_ts, end_date_ts);

        crow::json::wvalue response_data;
        response_data["result"] = std::move(resultArray);

        return make_success_response(200, response_data, "Analytics result retrieved.");

    }
    catch (const std::exception& e)
    {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::_get_complaints_grouped_by_field_over_time_get_cursor(std::shared_ptr<Database> db, const std::string& group_by_field, const bsoncxx::document::view& filter) -> mongocxx::cursor {
    mongocxx::pipeline pipeline{};
    pipeline.match(filter);

    pipeline.group(
        make_document(
            kvp("_id",
                make_document(
                    kvp("time_bucket",
                        make_document(
                            kvp("$dateToString",
                                make_document(
                                    kvp("format", "%m-%Y"),
                                    kvp("date", "$date")
                                )
                            )
                        )
                    ),
                    // e.g. "category" = "$category"
                    kvp(group_by_field, "$" + group_by_field)
                )
            ),
            kvp("count", make_document(kvp("$sum", 1))),
            kvp("avg_sentiment", make_document(kvp("$avg", "$sentiment")))
        )
    );

    auto cursor = db->aggregate(Constants::COLLECTION_COMPLAINTS, pipeline);
    return cursor;
}

auto ApiHandler::_get_complaints_grouped_by_field_over_time_read_cursor(mongocxx::cursor& cursor, const std::vector<std::string>& categories, const std::string& group_by_field, const long long int& start_date_ts, const long long int& end_date_ts) -> crow::json::wvalue::list {
    struct Stats {
        int count = 0;
        double avg = 0.0;
    };
    std::unordered_map<std::string, std::unordered_map<std::string, Stats>> aggregated;

    for (auto&& document : cursor)
    {
        auto doc_json = bsoncxx::to_json(document);
        auto rval_json = crow::json::load(doc_json);
        crow::json::wvalue wval_json;
        wval_json["count"] = rval_json["count"];
        wval_json["count"] = rval_json["count"];

        // Example structure of doc:
        // {
        //   "_id": {
        //       "time_bucket": "05-2023",
        //       "category": "Financial"
        //   },
        //   "count": 3,
        //   "avg_sentiment": 0.15
        // }
        auto time_bucket_val = rval_json["_id"]["time_bucket"].s();
        auto group_value     = rval_json["_id"][group_by_field].s();

        auto count_val       = rval_json["count"].i();
        auto avg_sent_val    = rval_json["avg_sentiment"].d();

        aggregated[time_bucket_val][group_value] = {static_cast<int>(count_val), static_cast<double>(avg_sent_val)};
    }

    auto start_tp = std::chrono::system_clock::time_point(std::chrono::milliseconds(start_date_ts));
    auto end_tp   = std::chrono::system_clock::time_point(std::chrono::milliseconds(end_date_ts));
    auto monthsInRange = _get_months_range(start_tp, end_tp);

    crow::json::wvalue::list resultArray;

    for (auto& monthStr : monthsInRange)
    {
        crow::json::wvalue monthObj;
        monthObj["date"] = monthStr;

        crow::json::wvalue dataObj;
        for (auto& cat : categories)
        {
            int theCount = 0;
            double theAvg = 0.0;

            auto itMonth = aggregated.find(monthStr);
            if (itMonth != aggregated.end())
            {
                auto itCategory = itMonth->second.find(cat);
                if (itCategory != itMonth->second.end())
                {
                    theCount = itCategory->second.count;
                    theAvg   = itCategory->second.avg;
                }
            }

            dataObj[cat]["count"] = theCount;
            dataObj[cat]["avg_sentiment"] = theAvg;
        }
        monthObj["data"] = std::move(dataObj);
        resultArray.push_back(std::move(monthObj));
    }

    return resultArray;
}

auto ApiHandler::get_complaints_grouped_by_sentiment_value(const crow::request& req, std::shared_ptr<Database> db) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"start_date", "end_date", "bucket_size"})) {
            return make_error_response(400, "Invalid request format");
        }

        auto start_date = json_date_to_bson_date(body["start_date"]);
        auto end_date = json_date_to_bson_date(body["end_date"]);

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

        auto cursor = _get_complaints_grouped_by_sentiment_value_get_cursor(db, bucket_size, filter);
        auto result = _get_complaints_grouped_by_sentiment_value_read_cursor(cursor, bucket_size);

        crow::json::wvalue response_data;
        response_data["result"] = std::move(result);
        return make_success_response(200, response_data, "Analytics result retrieved.");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::_get_complaints_grouped_by_sentiment_value_get_cursor(std::shared_ptr<Database> db, const double &bucket_size, const bsoncxx::document::view& filter) -> mongocxx::cursor {
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

    auto cursor = db->aggregate(Constants::COLLECTION_COMPLAINTS, pipeline);
    return cursor;
}

auto ApiHandler::_get_complaints_grouped_by_sentiment_value_read_cursor(mongocxx::cursor& cursor, const double& bucket_size) -> crow::json::wvalue {
    std::unordered_set<double> added_left_bounds;

    std::vector<crow::json::wvalue> result;
    for (auto&& doc : cursor) {
        auto doc_json = bsoncxx::to_json(doc);
        crow::json::rvalue rval_json = crow::json::load(doc_json);

        crow::json::wvalue sub_result;
        sub_result["left_bound_inclusive"] = rval_json["_id"];
        sub_result["right_bound_exclusive"] = rval_json["_id"].d() + bucket_size;
        sub_result["count"] = rval_json["count"];
        result.push_back(sub_result);
        added_left_bounds.insert(rval_json["_id"].d());
    }

    for (double left_bound = -1; left_bound < 1.01; left_bound += bucket_size) {
        if (added_left_bounds.find(left_bound) != added_left_bounds.end()) {
            continue;
        }
        crow::json::wvalue sub_result;
        sub_result["left_bound_inclusive"] = left_bound;
        sub_result["right_bound_exclusive"] = left_bound + bucket_size;
        sub_result["count"] = 0;
        result.push_back(sub_result);
        added_left_bounds.insert(left_bound);
    }

    return result;
}

auto ApiHandler::_get_group_by_field_all_distinct_values(std::shared_ptr<Database> db, const std::string& group_by_field) -> std::vector<std::string> {
    std::unordered_set<std::string> is_added;

    auto collection_name = _get_collection_name_from_group_by_field(group_by_field);

    auto views = db->find_all(collection_name);
    for (auto &view: views) {
        std::string val{view["name"].get_value().get_string().value};
        if (is_added.find(val) == is_added.end()) {
            is_added.insert(val);
        }
    }

    std::vector<std::string> result{is_added.begin(), is_added.end()};
    return result;
}

auto ApiHandler::_get_collection_name_from_group_by_field(const std::string& group_by_field) -> std::string {
    std::string collection_name = "";
    if (group_by_field == "category") {
        collection_name = "categories";
    } else if (group_by_field == "source") {
        collection_name = "sources";
    }
    return collection_name;
}

auto ApiHandler::_get_months_range(std::chrono::system_clock::time_point start_tp, std::chrono::system_clock::time_point end_tp) -> std::vector<std::string>
{
    struct YearMonth {
        int year;
        int month;
    };
    
    auto start_time_t = std::chrono::system_clock::to_time_t(start_tp);
    auto end_time_t   = std::chrono::system_clock::to_time_t(end_tp);

    std::tm start_tm = *std::gmtime(&start_time_t);
    std::tm end_tm   = *std::gmtime(&end_time_t);

    YearMonth startYM = {start_tm.tm_year + 1900, start_tm.tm_mon + 1};
    YearMonth endYM   = {end_tm.tm_year + 1900,   end_tm.tm_mon + 1};

    std::vector<std::string> result;
    if ((startYM.year > endYM.year) 
        || (startYM.year == endYM.year && startYM.month > endYM.month))
    {
        return result;
    }

    int year  = startYM.year;
    int month = startYM.month;
    while (true) {
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << month 
            << "-" 
            << year;
        result.push_back(oss.str());

        if (year == endYM.year && month == endYM.month) {
            break;
        }

        // Increment month
        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }

    return result;
}