#include "base_server.hpp"
#include <iostream>

BaseServer::BaseServer(int port, int concurrency)
    : port(port), concurrency(concurrency)
{
    define_handler_funcs();
}

void BaseServer::define_handler_funcs() {

}

void BaseServer::_register_handler_func(const std::string& route, const std::function<crow::response(const crow::request&)>& func, const crow::HTTPMethod& method) {
    HandlerFunc handler_func = {route, func, method};
    handler_funcs.push_back(handler_func);
}

void BaseServer::serve() {
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
