#ifndef REDDIT_MANAGER_H
#define REDDIT_MANAGER_H

#include "env_manager.hpp"

#include "crow.h"

#include <string>
#include <vector>

class RedditManager {
public:
    RedditManager(
        const std::string& reddit_api_id, 
        const std::string& reddit_api_secret, 
        const std::string& reddit_username, 
        const std::string& reddit_password, 
        const std::string& user_agent
    );

    static std::shared_ptr<RedditManager> create_from_env(EnvManager env_manager = EnvManager());

    std::vector<crow::json::wvalue> get_posts(const std::string& subreddit);

private:
    std::string reddit_api_id;
    std::string reddit_api_secret;
    std::string reddit_username;
    std::string reddit_password;
    std::string user_agent;

    std::string _get_access_token();
};

#endif // REDDIT_H
