#include "api_handler.hpp"

#include <cpr/cpr.h> 

auto ApiHandler::reddit_perform_update(const crow::request& req, std::shared_ptr<Database> db, const std::string& collection_name) -> crow::response {
    try {
        // auto body = crow::json::load(req.body);
        crow::json::wvalue response_data;
        return make_success_response(200, response_data, "Success");
    }
    catch (const std::exception& e) {
        return make_error_response(500, std::string("Server error: ") + e.what());
    }
}