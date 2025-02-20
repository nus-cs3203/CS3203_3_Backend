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

auto ApiHandler::get_all(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response {
    try {
        auto cursor = db->find(collection_name, {});

        std::vector<crow::json::wvalue> documents;
        for (auto&& document: cursor) {
            auto document_json = bsoncxx::to_json(document);
            documents.push_back(crow::json::load(document_json));
        }

        crow::json::wvalue response_data;
        response_data["documents"] = std::move(documents);
        return make_success_response(200, response_data, "Document(s) retrieved successfully");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::get_by_oid(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"oid"})) {
            return make_error_response(400, "Invalid request format");
        }

        std::string oid_str = body["oid"].s();
        bsoncxx::oid oid{oid_str};

        auto filter = make_document(
            kvp("_id", oid)
        );

        auto result = db->find_one(collection_name, filter);

        crow::json::wvalue response_data;
        response_data["document"] = {};

        if (!result.has_value()) {
            return make_success_response(200, response_data, "No matching documents found");
        }

        auto document_json = bsoncxx::to_json(result.value());
        auto document_wvalue = crow::json::load(document_json);

        response_data["document"] = document_wvalue;
        return make_success_response(200, response_data, "Document successfully");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::search(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name, const std::vector<std::string>& keys, const std::vector<bool>& ascending_orders) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"filter", "page_size", "page_number"})) {
            return make_error_response(400, "Invalid request format");
        }

        auto bson_filter = json_to_bson(body["filter"]);
        auto page_size = body["page_size"].i();
        auto page_number = body["page_number"].i();

        if (page_size < 1) {
            return make_error_response(400, "Invalid page_size < 1.");
        }
        if (page_number < 1) {
            return make_error_response(400, "Invalid page_number < 1.");
        }

        bsoncxx::builder::basic::document sort_builder{};
        for (int i = 0; i < keys.size(); ++i) {
            std::string key = keys[i];
            int direction = ascending_orders[i] ? 1 : -1;
            sort_builder.append(kvp(key, direction));
        }

        mongocxx::options::find find_options;
        find_options.sort(sort_builder.view());
        find_options.skip((page_number - 1) * page_size);
        find_options.limit(page_size);

        auto cursor = db->find(collection_name, bson_filter.view(), find_options);

        auto total_count = db->count_documents(collection_name, bson_filter.view());

        std::vector<crow::json::wvalue> documents;
        for (auto&& document: cursor) {
            auto document_json = bsoncxx::to_json(document);
            documents.push_back(crow::json::load(document_json));
        }

        crow::json::wvalue response_data;
        response_data["documents"] = std::move(documents);
        response_data["total_count"] = total_count;
        return make_success_response(200, response_data, "Documents retrieved successfully");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}


auto ApiHandler::insert_one(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"document"})) {
            return make_error_response(400, "Invalid request format");
        }

        auto json_document = body["document"];
        auto bson_document = json_to_bson(json_document);

        auto result = db->insert_one(collection_name, bson_document.view());
        auto id = result->inserted_id().get_oid().value.to_string();

        crow::json::wvalue response_data;
        response_data["_id"] = id;
        return make_success_response(200, response_data, "Document inserted successfully");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::delete_by_oid(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"oid"})) {
            return make_error_response(400, "Invalid request format");
        }

        std::string oid_str = body["oid"].s();
        bsoncxx::oid oid{oid_str};

        auto filter = make_document(
            kvp("_id", oid)
        );

        auto result = db->delete_one(collection_name, filter.view());
        auto deleted_count = result->deleted_count();

        crow::json::wvalue response_data;
        response_data["deleted_count"] = deleted_count;
        return make_success_response(200, response_data, "Document deleted successfully");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::delete_many_by_oids(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"oids"})) {
            return make_error_response(400, "Invalid request format");
        }

        if (body["oids"].t() != crow::json::type::List) {
            return make_error_response(400, "Invalid format: 'oids' must be an array");
        }

        auto oid_jsons = body["oids"];
        bsoncxx::builder::basic::array oid_arr_builder;
        for (const auto& oid_json: oid_jsons.lo()) {
            std::string oid_str = oid_json.s();
            bsoncxx::oid oid{oid_str};
            oid_arr_builder.append(oid);
        }

        bsoncxx::array::value oid_arr = oid_arr_builder.extract();

        auto filter = make_document(
            kvp(
                "_id",
                make_document(
                    kvp("$in", bsoncxx::types::b_array{oid_arr.view()})
                )
            )
        );

        auto result = db->delete_many(collection_name, filter.view());
        auto deleted_count = result->deleted_count();

        crow::json::wvalue response_data;
        response_data["deleted_count"] = deleted_count;
        return make_success_response(200, response_data, "Document(s) deleted successfully");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}

auto ApiHandler::update_by_oid(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response {
    try {
        auto body = crow::json::load(req.body);

        if (!validate_request(body, {"oid", "update_document"})) {
            return make_error_response(400, "Invalid request format");
        }

        std::string oid_str = body["oid"].s();
        bsoncxx::oid oid{oid_str};

        auto filter = make_document(
            kvp("_id", oid)
        );

        auto update_document = json_to_bson(body["update_document"]);

        auto result = db->update_one(collection_name, filter.view(), update_document.view(), false);

        auto matched_count = result->matched_count();
        auto modified_count = result->modified_count();
        auto upserted_count = result->upserted_count();

        crow::json::wvalue response_data;
        response_data["matched_count"] = matched_count;
        response_data["modified_count"] = modified_count;
        response_data["upserted_count"] = upserted_count;
        return make_success_response(200, response_data, "Document(s) updated successfully");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}