#ifndef UTILS_H
#define UTILS_H

#include <bsoncxx/builder/basic/document.hpp>
#include "crow.h"

auto json_to_bson(const crow::json::rvalue& json_document) -> bsoncxx::document::value;

#endif
