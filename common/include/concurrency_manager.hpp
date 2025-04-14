#ifndef CONCURRENCY_MANAGER_HPP
#define CONCURRENCY_MANAGER_HPP

#include <functional>
#include <memory>
#include <mutex>

#include "crow.h"

class ConcurrencyManager {
   public:
    auto concurrency_protection_decorator(
        const std::function<crow::response(const crow::request&)>& func)
        -> std::function<crow::response(const crow::request&)>;

   private:
    std::mutex mutex;
};

#endif
