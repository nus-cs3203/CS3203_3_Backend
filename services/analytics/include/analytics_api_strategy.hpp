#ifndef ANALYTICS_API_STRATEGY_H
#define ANALYTICS_API_STRATEGY_H

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <tuple>
#include <vector>

namespace AnalyticsApiStrategy {
    auto process_request_func_get_one_by_name(const crow::request& req) -> std::tuple<bsoncxx::document::value, mongocxx::options::find>; 
}

#endif