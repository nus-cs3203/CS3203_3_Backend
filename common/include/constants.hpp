#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

namespace Constants {
    const std::string DATETIME_FORMAT = "%d-%m-%Y %H:%M:%S";

    const std::string MONGO_URI = "mongodb://127.0.0.1:27017";
    const std::string DB_NAME = "CS3203";
    const std::string COLLECTION_COMPLAINTS = "complaints";
}

#endif // CONSTANTS_HPP
