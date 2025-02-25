#ifndef REDDIT_H
#define REDDIT_H

#include <string>

class Reddit {
public:
    Reddit(
        const std::string& reddit_api_id, 
        const std::string& reddit_api_secret, 
        const std::string& reddit_username, 
        const std::string& reddit_password, 
        const std::string& user_agent
    );

    static Reddit create_with_values_from_env();

private:
    std::string reddit_api_id;
    std::string reddit_api_secret;
    std::string reddit_username;
    std::string reddit_password;
    std::string user_agent;
};

#endif // REDDIT_H
