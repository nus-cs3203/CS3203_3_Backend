#include "utils.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"

#include <iostream>

auto json_to_bson(const crow::json::rvalue& json_document) -> bsoncxx::document::value {
    std::ostringstream oss;
    oss << json_document;
    std::string json_str = oss.str();
    return bsoncxx::from_json(json_str);
}
