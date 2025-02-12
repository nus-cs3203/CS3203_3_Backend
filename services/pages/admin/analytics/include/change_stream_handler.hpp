#ifndef CHANGE_STREAM_HANDLER_H
#define CHANGE_STREAM_HANDLER_H

#include "database.hpp"
#include "utils.hpp"

#include "crow.h"

class ChangeStreamHandler {
public:
    ChangeStreamHandler() = default;

    void handle_insert_one_post();
private:
};

#endif
