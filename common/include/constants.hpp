#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

namespace Constants {
    const std::string DATETIME_FORMAT = "%d-%m-%Y %H:%M:%S";

    const std::string MONGO_URI = "mongodb://127.0.0.1:27017";
    const std::string DB_NAME = "CS3203";

    const std::string ANALYTICS_URL = "";
    
    const std::string COLLECTION_CATEGORIES = "categories";
    const std::string COLLECTION_SOURCES = "sources";
    const std::string COLLECTION_POSTS = "posts";
    const std::string COLLECTION_COMPLAINTS = "complaints";
    const std::string COLLECTION_CATEGORY_ANALYTICS = "category_analytics";
    const std::string COLLECTION_USERS = "users";
    const std::string COLLECTION_POLLS = "polls";
    const std::string COLLECTION_POLL_TEMPLATES = "poll_templates";
    const std::string COLLECTION_POLL_REPONSES = "poll_responses";
    const std::string COLLECTION_ANALYTICS_TASK_IDS = "analytics_task_ids";

    const std::string REDDIT_API_ID = "";
    const std::string REDDIT_API_SECRET = "";
    const std::string REDDIT_USERNAME = "";
    const std::string REDDIT_PASSWORD = "";
    const std::string USER_AGENT = "";

    const std::string USERS_ROLE_CITIZEN = "Citizen";
    const std::string USERS_ROLE_ADMIN = "Admin";

    const std::string ENV_FILE_DEFAULT_PATH = "./.env";

    const int ANALYTICS_SERVER_PORT_NUMBER = 8082;
    const int MANAGEMENT_SERVER_PORT_NUMBER = 8083;
    const int USER_SERVER_PORT_NUMBER = 8085;
    const int DEFAULT_CONCURRENCY = 25;
}

#endif // CONSTANTS_HPP
