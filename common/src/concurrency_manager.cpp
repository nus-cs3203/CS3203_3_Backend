#include "concurrency_manager.hpp"

auto ConcurrencyManager::concurrency_protection_decorator(
    const std::function<crow::response(const crow::request&)>& func)
    -> std::function<crow::response(const crow::request&)> {
    return [this, func](const crow::request& req) {
        std::lock_guard<std::mutex> api_lock(this->mutex);
        return func(req);
    };
}
