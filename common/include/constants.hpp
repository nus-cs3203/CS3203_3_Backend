#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

namespace Constants {
    const std::string DATETIME_FORMAT = "%d-%m-%Y %H:%M:%S";

    const std::string MONGO_URI = "mongodb://127.0.0.1:27017";
    const std::string DB_NAME = "CS3203";
    
    const std::string COLLECTION_CATEGORIES = "categories";
    const std::string COLLECTION_SOURCES = "sources";
    const std::string COLLECTION_POSTS = "posts";
    const std::string COLLECTION_COMPLAINTS = "complaints";
    const std::string COLLECTION_CATEGORY_ANALYTICS = "category_analytics";
    const std::string COLLECTION_USERS = "users";
    const std::string COLLECTION_POLLS = "polls";
    const std::string COLLECTION_POLL_TEMPLATES = "poll_templates";
    const std::string COLLECTION_POLL_RESPONSES = "poll_responses";

    const std::string USERS_ROLE_CITIZEN = "Citizen";
    const std::string USERS_ROLE_ADMIN = "Admin";

    const std::string ENV_FILE_DEFAULT_PATH = "./.env";

    const std::string DEFAULT_JWT_SECRET = "";
    const std::string DEFAULT_JWT_DURATION_IN_SECONDS = "3600";

    const int ANALYTICS_SERVER_PORT_NUMBER = 8082;
    const int MANAGEMENT_SERVER_PORT_NUMBER = 8083;
    const int USER_SERVER_PORT_NUMBER = 8085;
    const int DEFAULT_CONCURRENCY = 25;
}

#endif // CONSTANTS_HPP
