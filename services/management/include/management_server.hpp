#ifndef MANAGEMENT_SERVER_H
#define MANAGEMENT_SERVER_H

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "base_server.hpp"
#include "concurrency_manager.hpp"
#include "constants.hpp"
#include "cors.hpp"
#include "crow.h"
#include "database_manager.hpp"
#include "management_api_handler.hpp"

class ManagementServer : public BaseServer {
   public:
    ManagementServer(int port = Constants::MANAGEMENT_SERVER_PORT_NUMBER,
                     int concurrency = Constants::DEFAULT_CONCURRENCY);

   private:
    void _define_handler_funcs();
};

#endif
