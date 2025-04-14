#include "date_utils.hpp"  

#include <gtest/gtest.h>
#include <chrono>
#include <ctime>
#include <stdexcept>
#include <string>

// ----- Test for get_utc_timestamp_now -----
TEST(DateUtilsTest, GetUtcTimestampNow) {
    // Capture current system time in seconds.
    auto sys_now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto timestamp_now = DateUtils::get_utc_timestamp_now();

    // Allow a tolerance of a few seconds in case there is a delay between calls.
    EXPECT_NEAR(timestamp_now, static_cast<long long>(sys_now), 2)
        << "get_utc_timestamp_now should return a timestamp close to system time.";
}

// ----- Test for get_utc_timestamp_one_day_ago -----
TEST(DateUtilsTest, GetUtcTimestampOneDayAgo) {
    // Calculate now and one day ago using the utility functions.
    auto timestamp_now = DateUtils::get_utc_timestamp_now();
    auto timestamp_one_day_ago = DateUtils::get_utc_timestamp_one_day_ago();

    // Due to the slight delay between the two calls, allow for a small difference.
    EXPECT_NEAR(timestamp_now - timestamp_one_day_ago, 86400, 2)
        << "The difference between now and one day ago should be approximately 86400 seconds.";
}

// ----- Test for extract_month_from_timestamp_str -----
TEST(DateUtilsTest, ExtractMonthFromTimestampStr) {
    // Example timestamp: "dd-mm-YYYY HH:MM:SS"
    std::string timestamp = "15-08-2025 10:20:30";
    int month = DateUtils::extract_month_from_timestamp_str(timestamp);
    EXPECT_EQ(month, 8)
        << "extract_month_from_timestamp_str should return 8 for the month \"08\" in the timestamp.";
}

// ----- Test for extract_year_from_timestamp_str -----
TEST(DateUtilsTest, ExtractYearFromTimestampStr) {
    std::string timestamp = "15-08-2025 10:20:30";
    int year = DateUtils::extract_year_from_timestamp_str(timestamp);
    EXPECT_EQ(year, 2025)
        << "extract_year_from_timestamp_str should return 2025 for the timestamp provided.";
}

// ----- Test for create_month_year_str -----
TEST(DateUtilsTest, CreateMonthYearStr) {
    // Test with a single-digit month.
    std::string result1 = DateUtils::create_month_year_str(3, 2023);
    EXPECT_EQ(result1, "03-2023")
        << "create_month_year_str should pad the month with 0 for single-digit months.";

    // Test with a two-digit month.
    std::string result2 = DateUtils::create_month_year_str(11, 2023);
    EXPECT_EQ(result2, "11-2023")
        << "create_month_year_str should correctly format two-digit months.";
}

// ----- Test for utc_unix_timestamp_to_string -----
TEST(DateUtilsTest, UtcUnixTimestampToString) {
    // Unix epoch (0 seconds since Jan 1, 1970)
    std::string formatted = DateUtils::utc_unix_timestamp_to_string(0, "%d-%m-%Y %H:%M:%S");
    EXPECT_EQ(formatted, "01-01-1970 00:00:00")
        << "utc_unix_timestamp_to_string should convert 0 to '01-01-1970 00:00:00' using the provided format.";
}

// ----- Test for string_to_utc_unix_timestamp (valid input) -----
TEST(DateUtilsTest, StringToUtcUnixTimestampValid) {
    // Using the same format as above.
    long long timestamp = DateUtils::string_to_utc_unix_timestamp("01-01-1970 00:00:00", "%d-%m-%Y %H:%M:%S");
    EXPECT_EQ(timestamp, 0)
        << "string_to_utc_unix_timestamp should convert '01-01-1970 00:00:00' to 0.";
}

// ----- Test for string_to_utc_unix_timestamp (invalid input) -----
TEST(DateUtilsTest, StringToUtcUnixTimestampInvalidThrows) {
    // The provided date string does not match the expected format.
    EXPECT_THROW(
        DateUtils::string_to_utc_unix_timestamp("invalid-date", "%d-%m-%Y %H:%M:%S"),
        std::runtime_error
    ) << "string_to_utc_unix_timestamp should throw a runtime_error for an improperly formatted date string.";
}
