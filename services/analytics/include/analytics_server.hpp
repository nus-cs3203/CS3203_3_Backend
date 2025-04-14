#ifndef ANALYTICS_SERVER_H
#define ANALYTICS_SERVER_H

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "analytics_api_handler.hpp"
#include "base_server.hpp"
#include "concurrency_manager.hpp"
#include "constants.hpp"
#include "cors.hpp"
#include "crow.h"
#include "database_manager.hpp"
#include "jwt_manager.hpp"

class AnalyticsServer : public BaseServer {
   public:
    AnalyticsServer(int port = Constants::ANALYTICS_SERVER_PORT_NUMBER,
                    int concurrency = Constants::DEFAULT_CONCURRENCY);

   private:
    void _define_handler_funcs();
};

#endif
