#include "base_server.hpp"
#include <iostream>

BaseServer::BaseServer(int port, int concurrency) : port(port), concurrency(concurrency) {}

void BaseServer::_register_handler_func(const std::string& route, const std::function<crow::response(const crow::request&)>& func, const crow::HTTPMethod& method, const std::function<handler_func_type(const handler_func_type&)>& concurrency_protection_decorator, const JwtAccessLevel& access_level, const std::function<handler_func_type(const handler_func_type&, const JwtAccessLevel& access_level)>& jwt_protection_decorator) {
    HandlerFunc handler_func = {route, func, method, concurrency_protection_decorator, access_level, jwt_protection_decorator};
    handler_funcs.push_back(handler_func);
}

void BaseServer::_decorate_handler_funcs() {
    for (auto &handler_func: handler_funcs) {
        handler_func.func = handler_func.jwt_protection_decorator(handler_func.func, handler_func.access_level);
        handler_func.func = handler_func.concurrency_protection_decorator(handler_func.func);
    }
}

void BaseServer::_init_server() {
    _define_handler_funcs();
    _decorate_handler_funcs();
}

void BaseServer::serve() {
    _init_server();

    bool should_restart = false;
    do {
        try {
            auto app = std::make_unique<crow::App<CORS>>();

            app->loglevel(crow::LogLevel::Warning);
            for (const auto& handler: handler_funcs) {
                app->route_dynamic(handler.route).methods(handler.method)(handler.func);
            }
            app->concurrency(concurrency);
            std::cout << "Server starting on port " << port << std::endl;
            app->port(port).run();

        } catch (const std::exception& e) {
            std::cout << "Server error: " << e.what() << std::endl;
            std::cout << "Restaring the server." << std::endl;
            should_restart = true;
        } 
    } while (should_restart);

    std::cout << "Stopping the server.";
}
