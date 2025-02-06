#include "utils.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include "crow.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

auto json_to_bson(const crow::json::rvalue& json_document) -> bsoncxx::document::value {
    std::ostringstream oss;
    oss << json_document;
    std::string json_str = oss.str();
    return bsoncxx::from_json(json_str);
}
