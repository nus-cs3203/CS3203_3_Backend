#ifndef USER_SERVER_H
#define USER_SERVER_H

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
#include "user_api_handler.hpp"

class UserServer : public BaseServer {
   public:
    UserServer(int port = Constants::USER_SERVER_PORT_NUMBER,
               int concurrency = Constants::DEFAULT_CONCURRENCY);

   private:
    void _define_handler_funcs();
};

#endif
