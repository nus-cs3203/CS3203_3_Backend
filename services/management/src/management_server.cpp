#include "management_server.hpp"

ManagementServer::ManagementServer(int port, int concurrency) : BaseServer(port, concurrency) {}

void ManagementServer::_define_handler_funcs() {
    auto api_handler = std::make_shared<ManagementApiHandler>();
    auto db_manager = DatabaseManager::create_from_env();
   
    auto concurrency_manager = std::make_shared<ConcurrencyManager>();
    auto concurrency_protection_decorator = [concurrency_manager](const std::function<crow::response(const crow::request&)> func) {
        return concurrency_manager->concurrency_protection_decorator(func);
    };

    const auto COLLECTION_CATEGORIES = Constants::COLLECTION_CATEGORIES;

    _register_handler_func(
        "/categories/get_count",
        [api_handler, db_manager, COLLECTION_CATEGORIES](const crow::request& req) { 
            return api_handler->count_documents(req, db_manager, COLLECTION_CATEGORIES);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/categories/get_all",
        [api_handler, db_manager, COLLECTION_CATEGORIES](const crow::request& req) { 
            return api_handler->get_all(req, db_manager, COLLECTION_CATEGORIES); 
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/categories/get_by_oid",
        [api_handler, db_manager, COLLECTION_CATEGORIES](const crow::request& req) { 
            return api_handler->get_one_by_oid(req, db_manager, COLLECTION_CATEGORIES);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/categories/insert_one",
        [api_handler, db_manager, COLLECTION_CATEGORIES](const crow::request& req) { 
            return api_handler->insert_one(req, db_manager, COLLECTION_CATEGORIES);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/categories/delete_by_oid",
        [api_handler, db_manager, COLLECTION_CATEGORIES](const crow::request& req) { 
            return api_handler->delete_one_by_oid(req, db_manager, COLLECTION_CATEGORIES); 
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/categories/update_by_oid",
        [api_handler, db_manager, COLLECTION_CATEGORIES](const crow::request& req) { 
            return api_handler->update_one_by_oid(req, db_manager, COLLECTION_CATEGORIES);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    const auto COLLECTION_POSTS = Constants::COLLECTION_POSTS;

    _register_handler_func(
        "/posts/get_count",
        [api_handler, db_manager, COLLECTION_POSTS](const crow::request& req) { 
            return api_handler->count_documents(req, db_manager, COLLECTION_POSTS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/posts/get_by_daterange",
        [api_handler, db_manager, COLLECTION_POSTS](const crow::request& req) { 
            return api_handler->get_by_daterange(req, db_manager, COLLECTION_POSTS); 
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    const auto COLLECTION_COMPLAINTS = Constants::COLLECTION_COMPLAINTS;

    _register_handler_func(
        "/complaints/get_count",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->count_documents(req, db_manager, COLLECTION_COMPLAINTS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/complaints/get_by_oid",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->get_one_by_oid(req, db_manager, COLLECTION_COMPLAINTS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/complaints/get_by_daterange",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->get_by_daterange(req, db_manager, COLLECTION_COMPLAINTS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/complaints/get_many",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->get_many(req, db_manager, COLLECTION_COMPLAINTS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/complaints/delete_by_oid",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->delete_one_by_oid(req, db_manager, COLLECTION_COMPLAINTS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/complaints/delete_many_by_oids",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->delete_many_by_oids(req, db_manager, COLLECTION_COMPLAINTS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/complaints/update_by_oid",
        [api_handler, db_manager, COLLECTION_COMPLAINTS](const crow::request& req) { 
            return api_handler->update_one_by_oid(req, db_manager, COLLECTION_COMPLAINTS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    const auto COLLECTION_POLLS = Constants::COLLECTION_POLLS;

    _register_handler_func(
        "/polls/insert_one",
        [api_handler, db_manager, COLLECTION_POLLS](const crow::request& req) { 
            return api_handler->insert_one(req, db_manager, COLLECTION_POLLS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/polls/get_by_oid",
        [api_handler, db_manager, COLLECTION_POLLS](const crow::request& req) { 
            return api_handler->get_one_by_oid(req, db_manager, COLLECTION_POLLS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/polls/get_many",
        [api_handler, db_manager, COLLECTION_POLLS](const crow::request& req) { 
            return api_handler->get_many(req, db_manager, COLLECTION_POLLS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/polls/get_count",
        [api_handler, db_manager, COLLECTION_POLLS](const crow::request& req) { 
            return api_handler->count_documents(req, db_manager, COLLECTION_POLLS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/polls/delete_by_oid",
        [api_handler, db_manager, COLLECTION_POLLS](const crow::request& req) { 
            return api_handler->delete_one_by_oid(req, db_manager, COLLECTION_POLLS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/polls/delete_many_by_oids",
        [api_handler, db_manager, COLLECTION_POLLS](const crow::request& req) { 
            return api_handler->delete_many_by_oids(req, db_manager, COLLECTION_POLLS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/polls/update_by_oid",
        [api_handler, db_manager, COLLECTION_POLLS](const crow::request& req) { 
            return api_handler->update_one_by_oid(req, db_manager, COLLECTION_POLLS);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    const auto COLLECTION_POLL_TEMPLATES = Constants::COLLECTION_POLL_TEMPLATES;

    _register_handler_func(
        "/poll_templates/get_all",
        [api_handler, db_manager, COLLECTION_POLL_TEMPLATES](const crow::request& req) { 
            return api_handler->get_all(req, db_manager, COLLECTION_POLL_TEMPLATES);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/poll_templates/get_by_oid",
        [api_handler, db_manager, COLLECTION_POLL_TEMPLATES](const crow::request& req) { 
            return api_handler->get_one_by_oid(req, db_manager, COLLECTION_POLL_TEMPLATES);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    const auto COLLECTION_POLL_RESPONSES = Constants::COLLECTION_POLL_RESPONSES;

    _register_handler_func(
        "/poll_responses/get_count",
        [api_handler, db_manager, COLLECTION_POLL_RESPONSES](const crow::request& req) { 
            return api_handler->count_documents(req, db_manager, COLLECTION_POLL_RESPONSES);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/poll_responses/get_one",
        [api_handler, db_manager, COLLECTION_POLL_RESPONSES](const crow::request& req) { 
            return api_handler->find_one(req, db_manager, COLLECTION_POLL_RESPONSES);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/poll_responses/get_many",
        [api_handler, db_manager, COLLECTION_POLL_RESPONSES](const crow::request& req) { 
            return api_handler->get_many(req, db_manager, COLLECTION_POLL_RESPONSES);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/poll_responses/insert_one",
        [api_handler, db_manager, COLLECTION_POLL_RESPONSES](const crow::request& req) { 
            return api_handler->insert_one(req, db_manager, COLLECTION_POLL_RESPONSES);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );

    _register_handler_func(
        "/poll_responses/get_statistics",
        [api_handler, db_manager, COLLECTION_POLL_RESPONSES](const crow::request& req) { 
            return api_handler->get_statistics_poll_responses(req, db_manager, COLLECTION_POLL_RESPONSES);
        },
        crow::HTTPMethod::Post,
        concurrency_protection_decorator
    );
}