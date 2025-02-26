#ifndef REDDIT_H
#define REDDIT_H

#include "crow.h"

#include <string>
#include <vector>

class Reddit {
public:
    Reddit(
        const std::string& reddit_api_id, 
        const std::string& reddit_api_secret, 
        const std::string& reddit_username, 
        const std::string& reddit_password, 
        const std::string& user_agent
    );

    std::string get_access_token();

    std::vector<crow::json::wvalue> get_posts(const std::string& subreddit, const int& limit, const long long int& start_utc_ts, const long long int& end_utc_ts);

    static Reddit create_with_values_from_env();

private:
    std::string reddit_api_id;
    std::string reddit_api_secret;
    std::string reddit_username;
    std::string reddit_password;
    std::string user_agent;
};

#endif // REDDIT_H
