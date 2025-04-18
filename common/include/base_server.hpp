#ifndef BASE_SERVER_H
#define BASE_SERVER_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "cors.hpp"
#include "crow.h"
#include "jwt_manager.hpp"

using handler_func_type = std::function<crow::response(const crow::request&)>;

struct HandlerFunc {
    std::string route;
    std::function<crow::response(const crow::request&)> func;
    crow::HTTPMethod method;
    std::function<handler_func_type(const handler_func_type&)> concurrency_protection_decorator;
    JwtAccessLevel access_level;
    std::function<handler_func_type(const handler_func_type&, const JwtAccessLevel& access_level)>
        jwt_protection_decorator;
};

class BaseServer {
   public:
    BaseServer(int port, int concurrency);
    void serve();

   protected:
    int port;
    int concurrency;
    std::vector<HandlerFunc> handler_funcs;

    void _init_server();
    virtual void _define_handler_funcs() = 0;
    void _decorate_handler_funcs();
    void _register_handler_func(const std::string& route,
                                const std::function<crow::response(const crow::request&)>& func,
                                const crow::HTTPMethod& method,
                                const std::function<handler_func_type(const handler_func_type&)>&
                                    concurrency_protection_decorator,
                                const JwtAccessLevel& access_level,
                                const std::function<handler_func_type(
                                    const handler_func_type&, const JwtAccessLevel& access_level)>&
                                    jwt_protection_decorator);
};

#endif
