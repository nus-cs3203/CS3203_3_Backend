#include "change_stream_handler.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include "crow.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <iostream>
#include <vector>

void ChangeStreamHandler::handle_insert_one_post() {
    const std::string uri_name = "mongodb://127.0.0.1:27017";
    const std::string db_name = "CS3203";
    const std::string collection_name = "posts";
    mongocxx::uri uri(uri_name);
    mongocxx::client client(uri);
    auto db = client[db_name];
    auto collection = db[collection_name];
    auto stream = collection.watch();
    while (true) {
        for (const auto& event : stream) {
            std::cout << bsoncxx::to_json(event) << std::endl;
            auto document = event["fullDocument"].get_document().view();
            auto category = document["category"].get_value().get_string().value;
            auto date = document["date"].get_value().get_string().value;
            auto sentiment = document["sentiment"].get_value().get_double().value;
            auto source = document["source"].get_value().get_string().value;
            auto title = document["title"].get_value().get_string().value;
            std::cout << category << " " << date << " " << sentiment << " " << source << " " << title << std::endl;

        }
    }
}