#include <cpr/cpr.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include "constants.hpp"
#include "database_manager.hpp"  // Your DatabaseManager header.
#include "management_server.hpp"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

// Utility function to clean a collection.
static inline void cleanup_collection(DatabaseManager& dbManager,
                                      const std::string& collection_name) {
    // Use an empty document to match all documents.
    dbManager.delete_many(collection_name, make_document());
}

// Test fixture for get endpoints.
class ManagementServerGetEndpointsTest : public ::testing::Test {
   protected:
    std::thread server_thread;
    int port = Constants::MANAGEMENT_SERVER_PORT_NUMBER;
    std::shared_ptr<DatabaseManager> db_ptr;

    void SetUp() override {
        db_ptr = std::make_shared<DatabaseManager>("mongodb://localhost:27017", "test_db");

        cleanup_collection(*db_ptr, "categories");
        cleanup_collection(*db_ptr, "complaints");
        cleanup_collection(*db_ptr, "posts");
        cleanup_collection(*db_ptr, "poll_templates");
        cleanup_collection(*db_ptr, "poll_responses");

        server_thread = std::thread([this]() {
            setenv("DB_NAME", "test_db", 1);
            ManagementServer server(port);
            server.serve();
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    void TearDown() override {
        cleanup_collection(*db_ptr, "categories");
        cleanup_collection(*db_ptr, "complaints");
        cleanup_collection(*db_ptr, "posts");
        cleanup_collection(*db_ptr, "poll_templates");
        cleanup_collection(*db_ptr, "poll_responses");

        if (server_thread.joinable()) {
            server_thread.detach();
        }
    }
};

// Test /categories/get_count endpoint.
TEST_F(ManagementServerGetEndpointsTest, CategoriesGetCount) {
    cleanup_collection(*db_ptr, "categories");

    auto doc1 = make_document(kvp("name", "Cat1"), kvp("color", "#111111"));
    auto doc2 = make_document(kvp("name", "Cat2"), kvp("color", "#222222"));
    db_ptr->insert_one("categories", doc1.view());
    db_ptr->insert_one("categories", doc2.view());

    std::string url = "http://localhost:" + std::to_string(port) + "/categories/get_count";
    std::string requestBody = R"json({
        "filter": {}
    })json";
    cpr::Response r = cpr::Post(cpr::Url{url}, cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    std::cout << r.text << std::endl;
    EXPECT_NE(r.text.find("\"count\":2"), std::string::npos);

    cleanup_collection(*db_ptr, "categories");
}

// Test /categories/get_all endpoint.
TEST_F(ManagementServerGetEndpointsTest, CategoriesGetAll) {
    cleanup_collection(*db_ptr, "categories");

    auto doc1 = make_document(kvp("name", "Cat1"), kvp("color", "#111111"));
    auto doc2 = make_document(kvp("name", "Cat2"), kvp("color", "#222222"));
    db_ptr->insert_one("categories", doc1.view());
    db_ptr->insert_one("categories", doc2.view());

    std::string url = "http://localhost:" + std::to_string(port) + "/categories/get_all";
    std::string requestBody = "{}";
    cpr::Response r = cpr::Post(cpr::Url{url}, cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    EXPECT_NE(r.text.find("Cat1"), std::string::npos);
    EXPECT_NE(r.text.find("Cat2"), std::string::npos);

    cleanup_collection(*db_ptr, "categories");
}

// Test /categories/get_by_oid endpoint.
TEST_F(ManagementServerGetEndpointsTest, CategoriesGetByOid) {
    cleanup_collection(*db_ptr, "categories");

    auto doc = make_document(kvp("name", "TestCategory"), kvp("color", "#ABCDEF"));

    auto result = db_ptr->insert_one("categories", doc.view());
    std::string oid = result.value().inserted_id().get_oid().value.to_string();

    std::string url = "http://localhost:" + std::to_string(port) + "/categories/get_by_oid";
    std::ostringstream bodyStream;
    bodyStream << "{ \"oid\": \"" << oid << "\" }";
    cpr::Response r = cpr::Post(cpr::Url{url}, cpr::Body{bodyStream.str()},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    EXPECT_NE(r.text.find("TestCategory"), std::string::npos);

    cleanup_collection(*db_ptr, "categories");
}

// Test /poll_templates/get_all endpoint.
TEST_F(ManagementServerGetEndpointsTest, PollTemplatesGetAll) {
    cleanup_collection(*db_ptr, "poll_templates");

    // Prepopulate "poll_templates" with two documents.
    auto doc1 =
        make_document(kvp("template_name", "Template1"), kvp("description", "First poll template"));
    auto doc2 = make_document(kvp("template_name", "Template2"),
                              kvp("description", "Second poll template"));
    db_ptr->insert_one("poll_templates", doc1.view());
    db_ptr->insert_one("poll_templates", doc2.view());

    // Call the endpoint.
    std::string url = "http://localhost:" + std::to_string(port) + "/poll_templates/get_all";
    std::string requestBody = "{}";
    cpr::Response r = cpr::Post(cpr::Url{url}, cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    // Verify that both templates appear in the response.
    EXPECT_NE(r.text.find("Template1"), std::string::npos);
    EXPECT_NE(r.text.find("Template2"), std::string::npos);

    cleanup_collection(*db_ptr, "poll_templates");
}

// Test /poll_responses/get_one endpoint.
TEST_F(ManagementServerGetEndpointsTest, PollResponsesGetOne) {
    cleanup_collection(*db_ptr, "poll_responses");

    auto doc = make_document(
        kvp("poll_id", "67da871c1447ef5cec00d5f1"), kvp("user_id", "67d93e8c3c0bfe14510691b1"),
        kvp("response", "Maxwell Food Centre"), kvp("date_submitted", "01-01-2022 00:00:00"));
    db_ptr->insert_one("poll_responses", doc.view());

    std::string url = "http://localhost:" + std::to_string(port) + "/poll_responses/get_one";
    std::string requestBody = R"json({
        "filter": {
            "poll_id": "67da871c1447ef5cec00d5f1",
            "user_id": "67d93e8c3c0bfe14510691b1"
        }
    })json";
    cpr::Response r = cpr::Post(cpr::Url{url}, cpr::Body{requestBody},
                                cpr::Header{{"Content-Type", "application/json"}});
    EXPECT_EQ(r.status_code, 200);
    EXPECT_NE(r.text.find("Maxwell Food Centre"), std::string::npos);
    cleanup_collection(*db_ptr, "poll_responses");
}
