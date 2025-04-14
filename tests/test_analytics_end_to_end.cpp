#include "analytics_server.hpp"
#include "constants.hpp"

#include <gtest/gtest.h>
#include <cpr/cpr.h>
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

// End-to-end test fixture that starts the AnalyticsServer in a background thread.
class AnalyticsServerE2ETest : public ::testing::Test {
protected:
    std::thread server_thread;
    int port = Constants::ANALYTICS_SERVER_PORT_NUMBER; 

    void SetUp() override {
        server_thread = std::thread([this]() {
            AnalyticsServer server(port);
            server.serve();
        });
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void TearDown() override {
        if (server_thread.joinable()) {
            server_thread.detach();
        }
    }
};

// -------- Test for /category_analytics/get_by_name --------
TEST_F(AnalyticsServerE2ETest, CategoryAnalyticsGetByName) {
    // Sample request from the README.
    std::string url = "http://localhost:" + std::to_string(port) + "/category_analytics/get_by_name";
    std::string requestBody = R"json({
        "name": "Housing"
    })json";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    // Check that the response contains the expected success message.
    EXPECT_NE(r.text.find("Server processed get request successfully"), std::string::npos);
}

// -------- Test for /complaints/get_statistics --------
TEST_F(AnalyticsServerE2ETest, ComplaintsGetStatistics) {
    std::string url = "http://localhost:" + std::to_string(port) + "/complaints/get_statistics";
    // Using a simple empty filter for a basic test.
    std::string requestBody = R"json({
        "filter": {}
    })json";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    EXPECT_NE(r.text.find("Server processed aggregate request successfully"), std::string::npos);
}

// -------- Test for /complaints/get_statistics_over_time --------
TEST_F(AnalyticsServerE2ETest, ComplaintsGetStatisticsOverTime) {
    std::string url = "http://localhost:" + std::to_string(port) + "/complaints/get_statistics_over_time";
    std::string requestBody = R"json({
        "filter": {
            "_from_date": "01-01-2023 00:00:00",
            "_to_date": "02-01-2023 00:00:00"
        }
    })json";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    EXPECT_NE(r.text.find("Server processed aggregate request successfully"), std::string::npos);
}

// -------- Test for /complaints/get_statistics_grouped --------
TEST_F(AnalyticsServerE2ETest, ComplaintsGetStatisticsGrouped) {
    std::string url = "http://localhost:" + std::to_string(port) + "/complaints/get_statistics_grouped";
    std::string requestBody = R"json({
        "group_by_field": "category",
        "filter": {}
    })json";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    EXPECT_NE(r.text.find("Server processed aggregate request successfully"), std::string::npos);
}

// -------- Test for /complaints/get_statistics_grouped_over_time --------
TEST_F(AnalyticsServerE2ETest, ComplaintsGetStatisticsGroupedOverTime) {
    std::string url = "http://localhost:" + std::to_string(port) + "/complaints/get_statistics_grouped_over_time";
    std::string requestBody = R"json({
        "group_by_field": "category",
        "filter": {
            "_from_date": "20-03-2024 00:00:00",
            "_to_date": "02-01-2025 23:59:59"
        }
    })json";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    EXPECT_NE(r.text.find("Server processed aggregate request successfully"), std::string::npos);
}

// -------- Test for /complaints/get_statistics_grouped_by_sentiment_value --------
TEST_F(AnalyticsServerE2ETest, ComplaintsGetStatisticsGroupedBySentimentValue) {
    std::string url = "http://localhost:" + std::to_string(port) + "/complaints/get_statistics_grouped_by_sentiment_value";
    std::string requestBody = R"json({
        "bucket_size": 0.5,
        "filter": {
            "_from_date": "01-01-2010 00:00:00",
            "_to_date": "31-12-2010 23:59:59"
        }
    })json";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    EXPECT_NE(r.text.find("Server processed aggregate request successfully"), std::string::npos);
}
