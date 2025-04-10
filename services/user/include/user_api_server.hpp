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
    UserApiServer(int port = 8085, int concurrency = Constants::DEFAULT_CONCURRENCY);
    void run();

private:
    std::unique_ptr<crow::App<CORS>> app;
    std::vector<HandlerFunc> handler_funcs;
    int port;
    int concurrency;

    void _define_handler_funcs();
    void _register_handler_func(const std::string& route,
                                const std::function<crow::response(const crow::request&)>& func,
                                const crow::HTTPMethod& method);
    void _init_server();
};

#endif
