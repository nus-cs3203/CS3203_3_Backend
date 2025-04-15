#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/options/aggregate.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/pipeline.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "analytics_api_strategy.hpp"
#include "crow.h"
#include "gtest/gtest.h"

using bsoncxx::to_json;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

// --------- Test for process_request_func_get_one_by_name ---------
TEST(AnalyticsApiStrategyTest, ProcessRequestGetOneByName) {
    crow::request req;
    std::string test_name = "TestName";
    req.body = "{\"name\": \"" + test_name + "\"}";

    auto result = AnalyticsApiStrategy::process_request_func_get_one_by_name(req);
    auto filter = std::get<0>(result);
    std::string filter_json = to_json(filter.view());

    // Verify that the filter document contains the "name" key with the correct value.
    EXPECT_NE(filter_json.find(test_name), std::string::npos);
}

// --------- Test for process_request_func_get_complaints_statistics ---------
TEST(AnalyticsApiStrategyTest, ProcessRequestGetComplaintsStatistics) {
    crow::request req;
    req.body = "{\"filter\": {\"complaint\": true}}";

    auto result = AnalyticsApiStrategy::process_request_func_get_complaints_statistics(req);
    auto documents = std::get<0>(result);
    // Expect two stages: filter and group.
    ASSERT_EQ(documents.size(), 2u);

    std::string filter_json = to_json(documents[0].view());
    std::string group_json = to_json(documents[1].view());

    // Verify the filter stage contains the expected filter.
    EXPECT_NE(filter_json.find("{ \"complaint\" : true }"), std::string::npos);
    // Verify the group stage contains $sum and $avg operators.
    EXPECT_NE(group_json.find("\"$sum\""), std::string::npos);
    EXPECT_NE(group_json.find("\"$avg\""), std::string::npos);
}

// --------- Test for process_request_func_get_complaints_statistics_over_time ---------
TEST(AnalyticsApiStrategyTest, ProcessRequestGetComplaintsStatisticsOverTime) {
    crow::request req;
    req.body =
        "{\"filter\": {\"_from_date\": \"01-01-2020 00:00:00\", \"_to_date\": \"31-12-2020 "
        "00:00:00\"}}";

    auto result =
        AnalyticsApiStrategy::process_request_func_get_complaints_statistics_over_time(req);
    auto documents = std::get<0>(result);
    ASSERT_EQ(documents.size(), 2u);

    std::string filter_json = to_json(documents[0].view());
    std::string group_json = to_json(documents[1].view());

    // Verify group stage contains operators for $year and $month.
    EXPECT_NE(group_json.find("$year"), std::string::npos);
    EXPECT_NE(group_json.find("$month"), std::string::npos);
}

// --------- Test for process_request_func_get_complaints_statistics_grouped ---------
TEST(AnalyticsApiStrategyTest, ProcessRequestGetComplaintsStatisticsGrouped) {
    crow::request req;
    req.body =
        "{\"group_by_field\": \"category\", \"filter\": {\"_from_date\": \"01-01-2020 00:00:00\", "
        "\"_to_date\": \"31-12-2020 00:00:00\"}}";

    auto result = AnalyticsApiStrategy::process_request_func_get_complaints_statistics_grouped(req);
    auto documents = std::get<0>(result);
    ASSERT_EQ(documents.size(), 2u);

    std::string group_json = to_json(documents[1].view());
    std::cout << group_json << std::endl;

    // Verify that the group stage groups by the field "category".
    EXPECT_NE(group_json.find("\"$category\""), std::string::npos);
    // Verify that the group stage contains an average sentiment calculation.
    EXPECT_NE(group_json.find("\"$avg\""), std::string::npos);
}

// --------- Test for process_request_func_get_complaints_statistics_grouped_over_time ---------
TEST(AnalyticsApiStrategyTest, ProcessRequestGetComplaintsStatisticsGroupedOverTime) {
    crow::request req;
    req.body =
        "{\"group_by_field\": \"category\", \"filter\": {\"_from_date\": \"01-01-2020 00:00:00\", "
        "\"_to_date\": \"31-12-2020 00:00:00\"}}";

    auto result =
        AnalyticsApiStrategy::process_request_func_get_complaints_statistics_grouped_over_time(req);
    auto documents = std::get<0>(result);
    ASSERT_EQ(documents.size(), 2u);

    std::string group_json = to_json(documents[1].view());
    std::cout << group_json << std::endl;
    // Verify that the grouping is done on year, month, and category.
    EXPECT_EQ(group_json,
              "{ \"_id\" : { \"year\" : { \"$year\" : \"$date\" }, \"month\" : { \"$month\" : "
              "\"$date\" }, \"category\" : \"$category\" }, \"count\" : { \"$sum\" : 1 }, "
              "\"avg_sentiment\" : { \"$avg\" : \"$sentiment\" } }");
}

// --------- Test for process_request_func_get_complaints_statistics_grouped_by_sentiment_value
// ---------
TEST(AnalyticsApiStrategyTest, ProcessRequestGetComplaintsStatisticsGroupedBySentimentValue) {
    crow::request req;
    req.body =
        "{\"filter\": {\"_from_date\": \"01-01-2020 00:00:00\", \"_to_date\": \"31-12-2020 "
        "00:00:00\"}, \"bucket_size\": 0.5}";

    auto result = AnalyticsApiStrategy::
        process_request_func_get_complaints_statistics_grouped_by_sentiment_value(req);
    auto documents = std::get<0>(result);
    ASSERT_EQ(documents.size(), 2u);

    std::string bucket_json = to_json(documents[1].view());

    std::cout << bucket_json << std::endl;
    // Verify that the bucket stage contains "boundaries", "default", and "output" fields.
    EXPECT_NE(bucket_json.find("boundaries"), std::string::npos);
    EXPECT_NE(bucket_json.find("OutOfRange"), std::string::npos);
    EXPECT_NE(bucket_json.find("output"), std::string::npos);
}

// --------- Test for _create_month_range ---------
TEST(AnalyticsApiStrategyTest, CreateMonthRange) {
    // Note: This function is declared in the header. We assume its implementation returns a vector
    // of (year, month) pairs.
    std::vector<std::pair<int, int>> monthRange =
        AnalyticsApiStrategy::_create_month_range("01-01-2022 00:00:00", "01-03-2022 00:00:00");

    // Expected result: January, February, and March of 2020.
    ASSERT_EQ(monthRange.size(), 3u);
    EXPECT_EQ(monthRange[0].first, 1);
    EXPECT_EQ(monthRange[0].second, 2022);
    EXPECT_EQ(monthRange[1].first, 2);
    EXPECT_EQ(monthRange[1].second, 2022);
    EXPECT_EQ(monthRange[2].first, 3);
    EXPECT_EQ(monthRange[2].second, 2022);
}

// --------- Test for GROUP_BY_FIELD_VALUES_MAPPER existence ---------
TEST(AnalyticsApiStrategyTest, GroupByFieldValuesMapperExists) {
    // Verify that the external mapper variable exists (even if empty).
    EXPECT_NO_THROW({
        auto mapper = AnalyticsApiStrategy::GROUP_BY_FIELD_VALUES_MAPPER;
        (void)mapper;
    });
}
