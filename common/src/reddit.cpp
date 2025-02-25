#include "reddit.hpp"
#include "utils.hpp"

#include <cpr/cpr.h>  

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

void Reddit::auth() {
    cpr::Authentication auth{reddit_api_id, reddit_api_secret, cpr::AuthMode::BASIC};
    cpr::Payload token_payload{
        {"grant_type", "password"},
        {"username",   reddit_username},
        {"password",   reddit_password}
    };
    cpr::Header base_headers{{"User-Agent", user_agent}};

    auto token_response = cpr::Post(
        cpr::Url{"https://www.reddit.com/api/v1/access_token"},
        auth,
        token_payload,
        base_headers
    );

    if (token_response.status_code != 200) {
        std::cerr << "Failed to obtain Reddit OAuth token. Status: " + std::to_string(token_response.status_code) << std::endl;
        return;
    }

    auto token_json = crow::json::load(token_response.text);
    std::cout << token_json << std::endl;
    if (!token_json || !token_json.has("access_token")) {
        std::cerr << "Reddit token response is missing 'access_token'" << std::endl;
        return;
    }
    auth_token = token_json["access_token"].s();
}
