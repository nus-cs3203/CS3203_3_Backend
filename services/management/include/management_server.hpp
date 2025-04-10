#ifndef MANAGEMENT_SERVER_H
#define MANAGEMENT_SERVER_H

#include "base_server.hpp"
#include "concurrency_manager.hpp"
#include "constants.hpp"
#include "cors.hpp"
#include "database_manager.hpp"
#include "management_api_handler.hpp"

#include "crow.h"

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class ManagementServer : public BaseServer {
public:
    ManagementServer(int port = Constants::MANAGEMENT_SERVER_PORT_NUMBER, int concurrency = Constants::DEFAULT_CONCURRENCY);

private:
    void _define_handler_funcs();
};

#endif
