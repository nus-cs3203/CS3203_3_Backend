#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <bsoncxx/builder/basic/document.hpp>
#include <string>
#include <vector>

#include "crow.h"

namespace DateUtils {
auto get_utc_timestamp_now() -> long long int;

auto get_utc_timestamp_one_day_ago() -> long long int;

auto extract_month_from_timestamp_str(const std::string& timestamp) -> int;

auto extract_year_from_timestamp_str(const std::string& timestamp) -> int;

auto create_month_year_str(const int& month, const int& year) -> std::string;

auto utc_unix_timestamp_to_string(const long long int& utc_unix_timestamp,
                                  const std::string& format) -> std::string;

auto string_to_utc_unix_timestamp(const std::string& datetime, const std::string& format)
    -> long long int;
}  // namespace DateUtils

#endif
