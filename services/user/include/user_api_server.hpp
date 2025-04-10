#ifndef USER_API_SERVER_H
#define USER_API_SERVER_H

#include "constants.hpp"
#include "cors.hpp"
#include "database_manager.hpp"
#include "user_api_handler.hpp"

#include "crow.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

struct HandlerFunc {
    std::string route;
    std::function<crow::response(const crow::request&)> func;
    crow::HTTPMethod method;
};

class UserApiServer {
public:
    UserApiServer();
    void init();
    void run(const int& port, const int& concurrency = Constants::DEFAULT_CONCURRENCY);
private:
    std::unique_ptr<crow::App<CORS>> app;
    std::unique_ptr<UserApiHandler> api_handler;
    std::shared_ptr<DatabaseManager> db_manager;

    std::string COLLECTION_USERS = Constants::COLLECTION_USERS;

    std::vector<HandlerFunc> handler_funcs;
    void _register_handler_funcs();
    void _register_handler_func(const std::string& route, const std::function<crow::response(const crow::request&)>& func, const crow::HTTPMethod& method);  
};

#endif