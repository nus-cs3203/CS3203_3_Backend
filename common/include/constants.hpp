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
    const std::string COLLECTION_POLL_REPONSES = "poll_responses";

    const std::string USERS_ROLE_CITIZEN = "CITIZEN";
    const std::string USERS_ROLE_ADMIN = "ADMIN";
    const std::string USERS_ROLE_SUPER_ADMIN = "SUPER_ADMIN";

    const std::string ENV_FILE_DEFAULT_PATH = "./.env";
}

#endif // CONSTANTS_HPP
