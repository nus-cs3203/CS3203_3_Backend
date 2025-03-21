#include "constants.hpp"
#include "date_utils.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

auto DateUtils::get_utc_timestamp_now() -> long long int {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

auto DateUtils::get_utc_timestamp_one_day_ago() -> long long int {
    return get_utc_timestamp_now() - 86400;
}

auto DateUtils::extract_month_from_timestamp_str(const std::string& timestamp) -> int {
    // dd-mm-YYYY HH:MM:SS
    return stoi(timestamp.substr(3, 2));
}

auto DateUtils::extract_year_from_timestamp_str(const std::string& timestamp) -> int {
    // dd-mm-YYYY HH:MM:SS
    return stoi(timestamp.substr(6, 4));
}

auto DateUtils::create_month_year_str(const int& month, const int& year) -> std::string {
    std::string month_str;
    if (month < 10) {
        month_str = "0" + std::to_string(month);
    } else {
        month_str = std::to_string(month);
    }
    std::string year_str = std::to_string(year);
    return month_str + "-" + year_str;
} 