#ifndef BASE_SERVER_H
#define BASE_SERVER_H

#include "cors.hpp"
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

class BaseServer {
public:
    BaseServer(int port, int concurrency);
    void serve();

protected:
    int port;
    int concurrency;
    std::vector<HandlerFunc> handler_funcs;

    void define_handler_funcs();
    void _register_handler_func(const std::string& route, const std::function<crow::response(const crow::request&)>& func, const crow::HTTPMethod& method);
};

#endif
