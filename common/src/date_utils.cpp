#include "date_utils.hpp"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "constants.hpp"

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

auto DateUtils::utc_unix_timestamp_to_string(const long long int& utc_unix_timestamp,
                                             const std::string& format) -> std::string {
    std::time_t time = static_cast<std::time_t>(utc_unix_timestamp);

    std::tm utc_time = *std::gmtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&utc_time, format.c_str());

    return oss.str();
}

auto DateUtils::string_to_utc_unix_timestamp(const std::string& datetime, const std::string& format)
    -> long long int {
    std::tm tm = {};
    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, format.c_str());

    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date/time string");
    }

    std::time_t time = timegm(&tm);
    return static_cast<long long int>(time);
}
