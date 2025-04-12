#include "management_server.hpp"
#include "constants.hpp"

#include <gtest/gtest.h>
#include <cpr/cpr.h>
#include <thread>
#include <chrono>
#include <sstream>
#include <iostream>

// End-to-end test fixture for the ManagementServer.
class ManagementServerE2ETest : public ::testing::Test {
protected:
    std::thread server_thread;
    int port = Constants::MANAGEMENT_SERVER_PORT_NUMBER;

    void SetUp() override {
        server_thread = std::thread([this]() {
            ManagementServer server(port);
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

// ===================
// Categories Endpoints
// ===================

// Test /categories/get_count endpoint.
TEST_F(ManagementServerE2ETest, CategoriesGetCount) {
    std::string url = "http://localhost:" + std::to_string(port) + "/categories/get_count";
    std::string requestBody = R"json({
        "filter": {}
    })json";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    // Verify that the response contains a success message or the count field.
    EXPECT_NE(r.text.find("Server processed count_documents request successfully"), std::string::npos);
}

// Test /categories/insert_one followed by /categories/get_by_oid.
TEST_F(ManagementServerE2ETest, CategoriesInsertGetUpdateDelete) {
    // -------- Insert a new category --------
    std::string insertUrl = "http://localhost:" + std::to_string(port) + "/categories/insert_one";
    std::string insertBody = R"json({
        "document": {
            "name": "TestCategory",
            "color": "#ABCDEF"
        }
    })json";

    cpr::Response insertResp = cpr::Post(cpr::Url{insertUrl},
                                          cpr::Body{insertBody},
                                          cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(insertResp.status_code, 200);
    // Extract the oid from the insert response (assuming it's returned in a field "oid")
    // For simplicity, we check that the response text contains an "oid" string.
    EXPECT_NE(insertResp.text.find("oid"), std::string::npos);

    // For the sake of testing, we assume the OID is returned as a JSON string.
    // Here we use a simplistic parsing approach.
    std::string insertedOid;
    {
        auto pos = insertResp.text.find("\"oid\"");
        ASSERT_NE(pos, std::string::npos);
        auto colonPos = insertResp.text.find(":", pos);
        auto quotePos1 = insertResp.text.find("\"", colonPos);
        auto quotePos2 = insertResp.text.find("\"", quotePos1 + 1);
        insertedOid = insertResp.text.substr(quotePos1 + 1, quotePos2 - quotePos1 - 1);
    }
    ASSERT_FALSE(insertedOid.empty());

    // -------- Get the newly inserted category using get_by_oid --------
    std::string getByOidUrl = "http://localhost:" + std::to_string(port) + "/categories/get_by_oid";
    std::ostringstream getByOidBodyStream;
    getByOidBodyStream << "{ \"oid\": \"" << insertedOid << "\" }";
    cpr::Response getByOidResp = cpr::Post(cpr::Url{getByOidUrl},
                                           cpr::Body{getByOidBodyStream.str()},
                                           cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(getByOidResp.status_code, 200);
    // Verify the returned document contains "TestCategory"
    EXPECT_NE(getByOidResp.text.find("TestCategory"), std::string::npos);

    // -------- Update the inserted category --------
    std::string updateUrl = "http://localhost:" + std::to_string(port) + "/categories/update_by_oid";
    std::ostringstream updateBodyStream;
    updateBodyStream << "{ \"oid\": \"" << insertedOid << "\", \"update_document\": { \"$set\": { \"color\": \"#123456\" } } }";
    cpr::Response updateResp = cpr::Post(cpr::Url{updateUrl},
                                         cpr::Body{updateBodyStream.str()},
                                         cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(updateResp.status_code, 200);
    EXPECT_NE(updateResp.text.find("Server processed update request successfully"), std::string::npos);

    // -------- Delete the category --------
    std::string deleteUrl = "http://localhost:" + std::to_string(port) + "/categories/delete_by_oid";
    std::ostringstream deleteBodyStream;
    deleteBodyStream << "{ \"oid\": \"" << insertedOid << "\" }";
    cpr::Response deleteResp = cpr::Post(cpr::Url{deleteUrl},
                                         cpr::Body{deleteBodyStream.str()},
                                         cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(deleteResp.status_code, 200);
    EXPECT_NE(deleteResp.text.find("Server processed delete request successfully"), std::string::npos);
}

// ===================
// Complaints Endpoints
// ===================

// Test /complaints/get_many endpoint with pagination and text search.
TEST_F(ManagementServerE2ETest, ComplaintsGetMany) {
    std::string url = "http://localhost:" + std::to_string(port) + "/complaints/get_many";
    std::string requestBody = R"json({
        "filter": {
            "$text": { "$search": "new" },
            "category": "Technology"
        },
        "page_size": 25,
        "page_number": 1
    })json";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    // Check that the response indicates success; adjust the expected substring if needed.
    EXPECT_NE(r.text.find("Server processed get request successfully"), std::string::npos);
}

// ===================
// Posts Endpoints
// ===================

// Test /posts/get_by_daterange endpoint.
TEST_F(ManagementServerE2ETest, PostsGetByDaterange) {
    std::string url = "http://localhost:" + std::to_string(port) + "/posts/get_by_daterange";
    std::string requestBody = R"json({
        "start_date": "01-01-2022 00:00:00",
        "end_date": "01-01-2022 23:59:59"
    })json";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    // Verify the response contains expected posts field (adjust based on actual response).
    EXPECT_NE(r.text.find("posts"), std::string::npos);
}

// ===================
// Polls Endpoints
// ===================

// Test /polls/insert_one then /polls/get_by_oid.
TEST_F(ManagementServerE2ETest, PollsInsertAndGetByOid) {
    // Insert a new poll.
    std::string insertUrl = "http://localhost:" + std::to_string(port) + "/polls/insert_one";
    std::string insertBody = R"json({
        "document": {
            "question": "Which hawker centre is the dirtiest?",
            "category": "Housing",
            "question_type": "MCQ",
            "options": ["Maxwell Food Centre", "Chinatown Complex", "Old Airport Road", "Newton Food Centre"],
            "date_created": "01-01-2022 00:00:00",
            "date_published": null,
            "date_closed": null,
            "status": "Unpublished"
        }
    })json";

    cpr::Response insertResp = cpr::Post(cpr::Url{insertUrl},
                                          cpr::Body{insertBody},
                                          cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(insertResp.status_code, 200);
    EXPECT_NE(insertResp.text.find("oid"), std::string::npos);

    // Extract the inserted OID as done before.
    std::string insertedOid;
    {
        auto pos = insertResp.text.find("\"oid\"");
        ASSERT_NE(pos, std::string::npos);
        auto colonPos = insertResp.text.find(":", pos);
        auto quotePos1 = insertResp.text.find("\"", colonPos);
        auto quotePos2 = insertResp.text.find("\"", quotePos1 + 1);
        insertedOid = insertResp.text.substr(quotePos1 + 1, quotePos2 - quotePos1 - 1);
    }
    ASSERT_FALSE(insertedOid.empty());

    // Get the inserted poll by OID.
    std::string getUrl = "http://localhost:" + std::to_string(port) + "/polls/get_by_oid";
    std::ostringstream getBodyStream;
    getBodyStream << "{ \"oid\": \"" << insertedOid << "\" }";
    cpr::Response getResp = cpr::Post(cpr::Url{getUrl},
                                      cpr::Body{getBodyStream.str()},
                                      cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(getResp.status_code, 200);
    EXPECT_NE(getResp.text.find("Server processed get request successfully"), std::string::npos);
}

// ===================
// Poll Templates Endpoints
// ===================

// Test /poll_templates/get_all endpoint.
TEST_F(ManagementServerE2ETest, PollTemplatesGetAll) {
    std::string url = "http://localhost:" + std::to_string(port) + "/poll_templates/get_all";
    std::string requestBody = "{}";

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    EXPECT_NE(r.text.find("Server processed get request successfully"), std::string::npos);
}

// ===================
// Poll Responses Endpoints
// ===================

// Test /poll_responses/insert_one and /poll_responses/get_one.
TEST_F(ManagementServerE2ETest, PollResponsesInsertAndGetOne) {
    // Insert a new poll response.
    std::string insertUrl = "http://localhost:" + std::to_string(port) + "/poll_responses/insert_one";
    std::string insertBody = R"json({
        "document": {
            "poll_id": "67da871c1447ef5cec00d5f1",
            "user_id": "67d93e8c3c0bfe14510691b1",
            "response": "Maxwell Food Centre",
            "date_submitted": "01-01-2022 00:00:00"
        }
    })json";

    cpr::Response insertResp = cpr::Post(cpr::Url{insertUrl},
                                          cpr::Body{insertBody},
                                          cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(insertResp.status_code, 200);
    EXPECT_NE(insertResp.text.find("oid"), std::string::npos);

    // Extract the inserted OID.
    std::string insertedOid;
    {
        auto pos = insertResp.text.find("\"oid\"");
        ASSERT_NE(pos, std::string::npos);
        auto colonPos = insertResp.text.find(":", pos);
        auto quotePos1 = insertResp.text.find("\"", colonPos);
        auto quotePos2 = insertResp.text.find("\"", quotePos1 + 1);
        insertedOid = insertResp.text.substr(quotePos1 + 1, quotePos2 - quotePos1 - 1);
    }
    ASSERT_FALSE(insertedOid.empty());

    // Get the poll response using a filter.
    std::string getUrl = "http://localhost:" + std::to_string(port) + "/poll_responses/get_one";
    std::string getBody = R"json({
        "filter": {
            "poll_id": "67da871c1447ef5cec00d5f1",
            "user_id": "67d93e8c3c0bfe14510691b1"
        }
    })json";

    cpr::Response getResp = cpr::Post(cpr::Url{getUrl},
                                      cpr::Body{getBody},
                                      cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(getResp.status_code, 200);
    EXPECT_NE(getResp.text.find("Server processed get request successfully"), std::string::npos);
}
