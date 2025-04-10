#ifndef USER_SERVER_H
#define USER_SERVER_H

#include "base_server.hpp"
#include "constants.hpp"
#include "cors.hpp"
#include "database_manager.hpp"
#include "user_api_handler.hpp"
#include "crow.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

class UserServer : public BaseServer {
public:
    UserServer(int port = Constants::USER_SERVER_PORT_NUMBER int concurrency = Constants::DEFAULT_CONCURRENCY);

private:
    void define_handler_funcs();
};

#endif
