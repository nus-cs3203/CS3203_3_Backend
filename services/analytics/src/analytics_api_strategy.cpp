#include "base_api_strategy_utils.hpp"
#include "analytics_api_strategy.hpp"
#include "date_utils.hpp"

#include <bsoncxx/json.hpp>
#include "crow.h"

#include <string>
#include <tuple>
#include <unordered_map>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto AnalyticsApiStrategy::process_request_func_get_one_by_name(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find> {
    BaseApiStrategyUtils::validate_fields(req, {"name"});

    auto body = crow::json::load(req.body);
    auto filter = make_document(
        kvp("name", body["name"].s())
    );

    mongocxx::options::find option;

    return std::make_tuple(filter, option);
}

auto AnalyticsApiStrategy::process_request_func_get_complaints_statistics(const crow::request& req) -> std::tuple<std::vector<bsoncxx::document::value>, mongocxx::options::aggregate> {
    BaseApiStrategyUtils::validate_fields(req, {"filter"});
    
    auto body = crow::json::load(req.body);
    auto filter = BaseApiStrategyUtils::parse_request_json_to_database_bson(body["filter"]);
    auto group = make_document(
        kvp("_id", bsoncxx::types::b_null()),
        kvp("count", make_document(kvp("$sum", 1))),
        kvp("avg_sentiment", make_document(kvp("$avg", "$sentiment")))
    );

    std::vector<bsoncxx::document::value> documents = {filter, group};

    mongocxx::options::aggregate option;

    return std::make_tuple(documents, option);
}

auto AnalyticsApiStrategy::process_request_func_get_complaints_statistics_over_time(const crow::request& req) -> std::tuple<std::vector<bsoncxx::document::value>, mongocxx::options::aggregate> {
    BaseApiStrategyUtils::validate_fields(req, {"filter"});
    
    auto body = crow::json::load(req.body);
    if (!body["filter"].has("_from_date")) {
        throw std::invalid_argument("Invalid request: missing _from_date field in filter");
    }
    if (!body["filter"].has("_to_date")) {
        throw std::invalid_argument("Invalid request: missing _to_date field in filter");
    }

    auto filter = BaseApiStrategyUtils::parse_request_json_to_database_bson(body["filter"]);
    auto group = make_document(
        kvp("_id", make_document(
            kvp("year", make_document(kvp("$year", "$date"))),
            kvp("month", make_document(kvp("$month", "$date")))
        )),
        kvp("count", make_document(kvp("$sum", 1))),
        kvp("avg_sentiment", make_document(kvp("$avg", "$sentiment")))
    );

    std::vector<bsoncxx::document::value> documents = {filter, group};

    mongocxx::options::aggregate option;

    return std::make_tuple(documents, option);
}

auto AnalyticsApiStrategy::process_request_func_get_complaints_statistics_grouped(const crow::request& req) -> std::tuple<std::vector<bsoncxx::document::value>, mongocxx::options::aggregate> {
    BaseApiStrategyUtils::validate_fields(req, {"group_by_field", "filter"});
    
    auto body = crow::json::load(req.body);
    auto filter = BaseApiStrategyUtils::parse_request_json_to_database_bson(body["filter"]);

    auto group_by_field = static_cast<std::string>(body["group_by_field"].s());
    auto group = make_document(
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
    );

    std::vector<bsoncxx::document::value> documents = {filter, group};

    mongocxx::options::aggregate option;

    return std::make_tuple(documents, option);
}

auto AnalyticsApiStrategy::process_request_func_get_complaints_statistics_grouped_over_time(const crow::request& req) -> std::tuple<std::vector<bsoncxx::document::value>, mongocxx::options::aggregate> {
    BaseApiStrategyUtils::validate_fields(req, {"group_by_field", "filter"});
    
    auto body = crow::json::load(req.body);
    if (!body["filter"].has("_from_date")) {
        throw std::invalid_argument("Invalid request: missing _from_date field in filter");
    }
    if (!body["filter"].has("_to_date")) {
        throw std::invalid_argument("Invalid request: missing _to_date field in filter");
    }

    auto filter = BaseApiStrategyUtils::parse_request_json_to_database_bson(body["filter"]);

    auto group_by_field = static_cast<std::string>(body["group_by_field"].s());
    auto group = make_document(
        kvp("_id",
            make_document(
                kvp("year", make_document(kvp("$year", "$date"))),
                kvp("month", make_document(kvp("$month", "$date"))),
                kvp(group_by_field, "$" + group_by_field)
            )
        ),
        kvp("count", make_document(kvp("$sum", 1))),
        kvp("avg_sentiment", make_document(kvp("$avg", "$sentiment")))
    );

    std::vector<bsoncxx::document::value> documents = {filter, group};

    mongocxx::options::aggregate option;

    return std::make_tuple(documents, option);
}

auto AnalyticsApiStrategy::create_pipeline_func_filter_and_group(const std::vector<bsoncxx::document::value>& documents) -> mongocxx::pipeline {
    mongocxx::pipeline pipeline{};

    const auto &filter = documents[0];
    const auto &group = documents[1];

    pipeline.match(filter.view());
    pipeline.group(group.view());

    return pipeline;
}

auto AnalyticsApiStrategy::process_response_func_get_complaints_statistics(const crow::request& req, mongocxx::cursor& cursor) -> crow::json::wvalue {
    crow::json::wvalue response_data;
    response_data["statistics"]["count"] = 0;
    response_data["statistics"]["avg_sentiment"] = 0;
    for (const auto& document: cursor) {
        auto document_json = bsoncxx::to_json(document);
        crow::json::rvalue rval_json = crow::json::load(document_json);
        response_data["statistics"]["count"] = rval_json["count"];
        response_data["statistics"]["avg_sentiment"] = rval_json["avg_sentiment"];
    }
    return response_data;
}

auto AnalyticsApiStrategy::process_response_func_get_complaints_statistics_over_time(const crow::request& req, mongocxx::cursor& cursor) -> crow::json::wvalue {
    auto body = crow::json::load(req.body);

    auto start_date = static_cast<std::string>(body["filter"]["_from_date"].s());
    auto end_date = static_cast<std::string>(body["filter"]["_to_date"].s());
    auto month_range = _create_month_range(start_date, end_date);

    struct Statistics {
        int count;
        double avg_sentiment;
    };
    
    std::map<std::pair<int, int>, Statistics> mapper;


    for (auto&& document: cursor) {
        auto doc_json = bsoncxx::to_json(document);
        auto doc_rval_json = crow::json::load(doc_json);

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
        wval_json["date"] = DateUtils::create_month_year_str(month, year);
        wval_json["data"]["count"] = stat.count;
        wval_json["data"]["avg_sentiment"] = stat.avg_sentiment;
        result.push_back(std::move(wval_json));
    }

    crow::json::wvalue response_data;
    response_data["statistics"] = std::move(result);
    return response_data;
}

auto AnalyticsApiStrategy::process_response_func_get_complaints_statistics_grouped(const crow::request& req, mongocxx::cursor& cursor) -> crow::json::wvalue {
    std::unordered_set<std::string> exists;

    crow::json::wvalue result;
    for (auto&& document: cursor) {
        auto document_json = bsoncxx::to_json(document);
        crow::json::rvalue rval_json = crow::json::load(document_json);

        crow::json::wvalue sub_result;
        sub_result["count"] = rval_json["count"];
        sub_result["avg_sentiment"] = rval_json["avg_sentiment"];
        auto group_by_field_value = rval_json["_id"].s();
        result[group_by_field_value] = std::move(sub_result);
        exists.insert(group_by_field_value);
    }

    auto body = crow::json::load(req.body);
    auto group_by_field = static_cast<std::string>(body["group_by_field"].s());
    auto group_by_field_values = AnalyticsApiStrategy::GROUP_BY_FIELD_VALUES_MAPPER[group_by_field];
    for (const auto &group_by_field_value: group_by_field_values) {
        if (exists.find(group_by_field_value) != exists.end()) {
            continue;
        }
        crow::json::wvalue sub_result;
        sub_result["count"] = 0;
        sub_result["avg_sentiment"] = 0;
        result[group_by_field_value] = std::move(sub_result);
        exists.insert(group_by_field_value);
    }

    crow::json::wvalue response_data;
    response_data["statistics"] = std::move(result);
    return response_data;
}

auto AnalyticsApiStrategy::process_response_func_get_complaints_statistics_grouped_over_time(const crow::request& req, mongocxx::cursor& cursor) -> crow::json::wvalue {
    auto body = crow::json::load(req.body);
    
    auto group_by_field = static_cast<std::string>(body["group_by_field"].s());
    auto start_date = static_cast<std::string>(body["filter"]["_from_date"].s());
    auto end_date = static_cast<std::string>(body["filter"]["_to_date"].s());
    auto month_range = _create_month_range(start_date, end_date);
    
    std::map<std::pair<int, int>, crow::json::wvalue> mapper;    
    
    for (auto&& document : cursor) {
        auto doc_json = bsoncxx::to_json(document);
        auto rval_json = crow::json::load(doc_json);

        std::string group_by_field_value = rval_json["_id"][group_by_field].s();
        int month = rval_json["_id"]["month"].i();
        int year = rval_json["_id"]["year"].i();
        
        auto month_year = std::make_pair(month, year);
        if (mapper.find(month_year) == mapper.end()) {
            crow::json::wvalue data;
            mapper[month_year] = std::move(data);
        }

        mapper[month_year][group_by_field_value]["count"] = rval_json["count"];
        mapper[month_year][group_by_field_value]["avg_sentiment"] = rval_json["avg_sentiment"];
    }

    const auto &group_by_field_values = AnalyticsApiStrategy::GROUP_BY_FIELD_VALUES_MAPPER[group_by_field];

    crow::json::wvalue result;
    for (const auto& month_year: month_range) {
        if (mapper.find(month_year) == mapper.end()) {
            mapper[month_year] = crow::json::wvalue{};
        }

        auto rval_json = crow::json::load(mapper[month_year].dump());

        int month = month_year.first;
        int year = month_year.second;
        
        auto month_year_str = DateUtils::create_month_year_str(month, year);
        for (const auto &group_by_field_value: group_by_field_values) {
            result[month_year_str][group_by_field_value]["count"] = 0;
            result[month_year_str][group_by_field_value]["avg_sentiment"] = 0;

            if (rval_json.t() != crow::json::type::Null and rval_json.has(group_by_field_value)) {
                result[month_year_str][group_by_field_value]["count"] = rval_json[group_by_field_value]["count"];
                result[month_year_str][group_by_field_value]["avg_sentiment"] = rval_json[group_by_field_value]["avg_sentiment"];
            }
        }
    }

    crow::json::wvalue response_data;
    response_data["statistics"] = std::move(result);
    return response_data;
}

auto AnalyticsApiStrategy::_create_month_range(const std::string& start_date, const std::string& end_date) -> std::vector<std::pair<int, int>> {
    int start_month = DateUtils::extract_month_from_timestamp_str(start_date);
    int start_year = DateUtils::extract_year_from_timestamp_str(start_date);

    int end_month = DateUtils::extract_month_from_timestamp_str(end_date);
    int end_year = DateUtils::extract_year_from_timestamp_str(end_date);

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

std::unordered_map<std::string, std::vector<std::string>> AnalyticsApiStrategy::GROUP_BY_FIELD_VALUES_MAPPER = {
    {"category", {"Housing", "Healthcare", "Public Safety", "Transport", "Education", "Environment", "Employment", "Public Health", "Legal", "Economy", "Politics", "Technology", "Infrastructure", "Others"}},
    {"source", {"Reddit"}}
};