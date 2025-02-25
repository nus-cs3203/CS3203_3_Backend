#include "reddit.hpp"
#include "utils.hpp"

#include <string>

Reddit::Reddit(
    const std::string& reddit_api_id, 
    const std::string& reddit_api_secret, 
    const std::string& reddit_username, 
    const std::string& reddit_password, 
    const std::string& user_agent
) 
    : 
        reddit_api_id(reddit_api_id), 
        reddit_api_secret(reddit_api_secret), 
        reddit_username(reddit_username), 
        reddit_password(reddit_password), 
        user_agent(user_agent) 
{}

Reddit Reddit::create_with_values_from_env() {
    return Reddit(
        read_env("REDDIT_API_ID", ""),
        read_env("REDDIT_API_SECRET", ""),
        read_env("REDDIT_USERNAME", ""),
        read_env("REDDIT_PASSWORD", ""),
        read_env("USER_AGENT", "")
    );
}
