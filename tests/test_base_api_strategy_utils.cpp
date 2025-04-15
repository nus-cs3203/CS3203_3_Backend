#include <crow.h>
#include <gtest/gtest.h>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <stdexcept>
#include <string>
#include <vector>

#include "base_api_strategy_utils.hpp"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

// ----- Test for validate_fields -----

// Test that a valid JSON body containing all required fields passes without throwing.
TEST(ValidateFieldsTest, ValidRequest) {
    crow::request req;
    req.body = "{\"username\":\"user1\", \"password\":\"secret\"}";
    // Should not throw because both required fields are present.
    EXPECT_NO_THROW(BaseApiStrategyUtils::validate_fields(req, {"username", "password"}));
}

// Test that an empty body throws an exception when there are required fields.
TEST(ValidateFieldsTest, EmptyBodyThrows) {
    crow::request req;
    req.body = "";
    EXPECT_THROW(BaseApiStrategyUtils::validate_fields(req, {"username"}), std::invalid_argument);
}

// Test that a body missing one required field throws an exception.
TEST(ValidateFieldsTest, MissingFieldThrows) {
    crow::request req;
    req.body = "{\"username\":\"user1\"}";
    try {
        BaseApiStrategyUtils::validate_fields(req, {"username", "password"});
        FAIL() << "Expected std::invalid_argument";
    } catch (const std::invalid_argument& ex) {
        std::string errMsg(ex.what());
        EXPECT_NE(errMsg.find("missing password"), std::string::npos)
            << "Error message should mention the missing field";
    } catch (...) {
        FAIL() << "Expected std::invalid_argument";
    }
}

// ----- Test for parse_database_json_to_response_json -----

// This function is expected to convert embedded document dates into strings.
TEST(ParseDatabaseJsonToResponseJsonTest, ConvertsDateField) {
    // Create input JSON that includes a "$date" field.
    // For example: { "created": { "$date": 1609459200000 } }
    auto input = crow::json::load("{\"created\":{\"$date\":0}}");
    ASSERT_TRUE(input);  // Ensure JSON loaded successfully.

    auto output = BaseApiStrategyUtils::parse_database_json_to_response_json(input);
    auto output_rval = crow::json::load(output.dump());
    // Check that the "created" field has been converted to a date string.
    EXPECT_EQ(output_rval["created"].t(), crow::json::type::String);

    std::string created = output_rval["created"].s();
    EXPECT_EQ(created, "01-01-1970 00:00:00");
}

// ----- Test for parse_request_json_to_database_bson -----
// Here we test a conversion of a simple JSON with both primitives and nested objects/arrays.

TEST(ParseRequestJsonToDatabaseBsonTest, ConvertsPrimitiveField) {
    // Input JSON: an object containing primitive types.
    auto input = crow::json::load("{\"username\":\"user1\", \"age\":30}");
    ASSERT_TRUE(input);

    auto output_doc = BaseApiStrategyUtils::parse_request_json_to_database_bson(input);
    auto expected_doc = make_document(kvp("username", "user1"), kvp("age", 30));

    EXPECT_EQ(bsoncxx::to_json(output_doc), bsoncxx::to_json(expected_doc));
}

// Test for Conversion of Nested Objects
TEST(ParseRequestJsonToDatabaseBsonTest, ConvertsNestedObject) {
    // Input JSON with a nested object.
    auto input = crow::json::load("{\"user\":{\"name\":\"user1\", \"age\":30}}");
    ASSERT_TRUE(input);

    auto output_doc = BaseApiStrategyUtils::parse_request_json_to_database_bson(input);

    // Build the expected nested document.
    auto nested_doc = make_document(kvp("name", "user1"), kvp("age", 30));
    auto expected_doc = make_document(kvp("user", nested_doc.view()));

    EXPECT_EQ(bsoncxx::to_json(output_doc), bsoncxx::to_json(expected_doc));
}

// Test for Conversion of Arrays
TEST(ParseRequestJsonToDatabaseBsonTest, ConvertsArray) {
    // Input JSON with an array.
    auto input = crow::json::load("{\"values\": [1, 2, 3]}");
    ASSERT_TRUE(input);

    auto output_doc = BaseApiStrategyUtils::parse_request_json_to_database_bson(input);

    // Build an expected array using the BSON builder.
    bsoncxx::builder::basic::array arr_builder;
    arr_builder.append(1);
    arr_builder.append(2);
    arr_builder.append(3);
    bsoncxx::array::value arr_value = arr_builder.extract();

    auto expected_doc = make_document(kvp("values", bsoncxx::types::b_array{arr_value.view()}));

    EXPECT_EQ(bsoncxx::to_json(output_doc), bsoncxx::to_json(expected_doc));
}

// Test for Conversion of Keys with LTE Prefix
TEST(ParseRequestJsonToDatabaseBsonTest, ConvertsInequalityOperatorLTE) {
    // Assume LTE_SIGN is defined as "<=" in your project.
    // Input JSON with a key starting with the LTE prefix.
    auto input = crow::json::load("{\"_to_age\": 30}");
    ASSERT_TRUE(input);

    auto output_doc = BaseApiStrategyUtils::parse_request_json_to_database_bson(input);

    // Expected: the key "age" mapped to a document with "$lte" operator.
    auto expected_doc = make_document(kvp("age", make_document(kvp("$lte", 30))));

    EXPECT_EQ(bsoncxx::to_json(output_doc), bsoncxx::to_json(expected_doc));
}

// Test for Conversion of Keys with GTE Prefix
TEST(ParseRequestJsonToDatabaseBsonTest, ConvertsInequalityOperatorGTE) {
    // Assume GTE_SIGN is defined as ">=" in your project.
    // Input JSON with a key starting with the GTE prefix.
    auto input = crow::json::load("{\"_from_age\": 30}");
    ASSERT_TRUE(input);

    auto output_doc = BaseApiStrategyUtils::parse_request_json_to_database_bson(input);

    // Expected: the key "age" mapped to a document with "$gte" operator.
    auto expected_doc = make_document(kvp("age", make_document(kvp("$gte", 30))));

    EXPECT_EQ(bsoncxx::to_json(output_doc), bsoncxx::to_json(expected_doc));
}

// ----- Test for parse_oid_str_to_oid_bson -----
// This function converts an OID string to a bson document.

TEST(ParseOidStrToOidBsonTest, ConvertsOidString) {
    std::string oid_str = "507f1f77bcf86cd799439011";
    auto bson_doc = BaseApiStrategyUtils::parse_oid_str_to_oid_bson(oid_str);
    auto view = bson_doc.view();

    auto elem = view["_id"];
    EXPECT_EQ(elem.type(), bsoncxx::type::k_oid);
    // Convert the bson oid back to a string to verify.
    EXPECT_EQ(elem.get_oid().value.to_string(), oid_str);
}

// ----- Test for parse_date_str_to_date_bson -----
// This function should convert a date string to a bson date object.

TEST(ParseDateStrToDateBsonTest, ConvertsDateString) {
    // Example date string. Ensure it matches your Constants::DATETIME_FORMAT.
    std::string date_str = "01-01-1970 00:00:00";
    auto bson_date = BaseApiStrategyUtils::parse_date_str_to_date_bson(date_str);

    auto ms_count = bson_date.to_int64();
    EXPECT_EQ(ms_count, 0);
}

// ----- Test for make_error_response -----

TEST(MakeErrorResponseTest, ReturnsErrorResponse) {
    int status_code = 400;
    std::string error_msg = "Bad Request";
    auto response = BaseApiStrategyUtils::make_error_response(status_code, error_msg);

    // Check that the response uses the expected status code.
    EXPECT_EQ(response.code, status_code);

    // Convert response body to a crow::json::rvalue.
    auto json_body = crow::json::load(response.body);
    ASSERT_TRUE(json_body);
    // Verify the JSON contains "success": false and the proper message.
    EXPECT_FALSE(json_body["success"].b());
    EXPECT_EQ(json_body["message"].s(), error_msg);
}

// ----- Test for make_success_response -----

TEST(MakeSuccessResponseTest, ReturnsSuccessResponse) {
    int status_code = 200;
    crow::json::wvalue data;
    data["data_field"] = "value1";
    std::string success_msg = "Operation successful";

    auto response = BaseApiStrategyUtils::make_success_response(status_code, data, success_msg);
    EXPECT_EQ(response.code, status_code);

    // Get the JSON from the response.
    auto json_body = crow::json::load(response.body);
    ASSERT_TRUE(json_body);

    // Verify that "success" is true, "message" is set correctly,
    // and that the original data field remains.
    EXPECT_TRUE(json_body["success"].b());
    EXPECT_EQ(json_body["message"].s(), success_msg);
    EXPECT_EQ(json_body["data_field"].s(), "value1");
}