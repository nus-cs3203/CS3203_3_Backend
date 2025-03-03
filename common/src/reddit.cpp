#include "reddit.hpp"
#include "utils.hpp"

#include <cpr/cpr.h>  
#include "crow.h"

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

std::string Reddit::_get_access_token() {
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
        throw std::runtime_error("Failed to obtain Reddit OAuth token. Status: " + std::to_string(token_response.status_code));
    }

    auto token_json = crow::json::load(token_response.text);
    if (!token_json || !token_json.has("access_token")) {
        throw std::runtime_error("Reddit token response is missing 'access_token'");
    }

    std::string access_token = token_json["access_token"].s();
    return access_token;
}

std::vector<crow::json::wvalue> Reddit::get_posts(const std::string& subreddit, const int& limit, const long long int& start_utc_ts, const long long int& end_utc_ts) {
    cpr::Header base_headers{{"User-Agent", user_agent}};
    cpr::Header oauth_headers = base_headers;
    std::string access_token = _get_access_token();
    oauth_headers["Authorization"] = "bearer " + access_token;

    auto listing_response = cpr::Get(
        cpr::Url{"https://oauth.reddit.com/r/" + subreddit + "/new?limit=" + std::to_string(limit)},
        oauth_headers
    );

    if (listing_response.status_code != 200) {
        throw std::runtime_error("Failed to retrieve data from /r/" + subreddit + "/new. Status code: " + std::to_string(listing_response.status_code));
    }

    auto listing_json = crow::json::load(listing_response.text);
    if (!listing_json || !listing_json.has("data")) {
        throw std::runtime_error("Invalid or unexpected JSON for /r/" + subreddit + "/new");
    }

    auto children = listing_json["data"]["children"];
    if (!children || children.t() != crow::json::type::List) {
        throw std::runtime_error("Missing 'children' array in /r/" + subreddit + "/new JSON");
    }

    std::vector<crow::json::wvalue> posts_array;
    posts_array.reserve(children.lo().size());

    for (auto& child : children.lo()) {
        if (!child.has("data")) continue;
        auto post_data = child["data"];
        if (!post_data) continue;

        crow::json::wvalue single_post;

        long long int now_ts = 0;
        if (post_data.has("created_utc") && post_data["created_utc"].t() == crow::json::type::Number) {
            now_ts = static_cast<long long int>(post_data["created_utc"].i());
        }
        if (now_ts < start_utc_ts or now_ts > end_utc_ts) {
            continue;
        }
        single_post["created_utc"] = now_ts;
        
        single_post["author_flair_text"] = "";
        if (post_data.has("author_flair_text") && post_data["author_flair_text"].t() == crow::json::type::String) {
            single_post["author_flair_text"] = post_data["author_flair_text"].s();
        }

        single_post["downs"] = 0;
        if (post_data.has("downs") && post_data["downs"].t() == crow::json::type::Number) {
            single_post["downs"] = post_data["downs"].i();
        }

        single_post["likes"] = 0;
        if (post_data.has("likes") && post_data["likes"].t() == crow::json::type::Number) {
            single_post["likes"] = post_data["likes"].i();
        }

        single_post["name"] = "";
        if (post_data.has("name") && post_data["name"].t() == crow::json::type::String) {
            single_post["name"] = post_data["name"].s();
        }

        single_post["no_follow"] = false;
        if (post_data.has("no_follow") && post_data["no_follow"].t() == crow::json::type::True) {
            single_post["no_follow"] = post_data["no_follow"].b();
        }

        single_post["num_comments"] = 0;
        if (post_data.has("num_comments") && post_data["num_comments"].t() == crow::json::type::Number) {
            single_post["num_comments"] = post_data["num_comments"].i();
        }

        single_post["score"] = 0;
        if (post_data.has("score") && post_data["score"].t() == crow::json::type::Number) {
            single_post["score"] = post_data["score"].i();
        }

        single_post["selftext"] = "";
        if (post_data.has("selftext") && post_data["selftext"].t() == crow::json::type::String) {
            single_post["selftext"] = post_data["selftext"].s();
        }

        single_post["title"] = "";
        if (post_data.has("title") && post_data["title"].t() == crow::json::type::String) {
            single_post["title"] = post_data["title"].s();
        }

        single_post["ups"] = 0;
        if (post_data.has("ups") && post_data["ups"].t() == crow::json::type::Number) {
            single_post["ups"] = post_data["ups"].i();
        }

        single_post["upvote_ratio"] = 0.0;
        if (post_data.has("upvote_ratio") && post_data["upvote_ratio"].t() == crow::json::type::Number) {
            single_post["upvote_ratio"] = post_data["upvote_ratio"].d();
        }

        single_post["url"] = "";
        if (post_data.has("url") && post_data["url"].t() == crow::json::type::String) {
            single_post["url"] = post_data["url"].s();
        }

        single_post["view_count"] = 0;
        if (post_data.has("view_count") && post_data["view_count"].t() == crow::json::type::Number) {
            single_post["view_count"] = post_data["view_count"].i();
        }

        if (!post_data.has("id") or post_data["id"].t() != crow::json::type::String) {
            single_post["comments"] = "";
            posts_array.push_back(single_post);
            continue;
        }
        std::string short_id = post_data["id"].s();

        std::string comments_url = "https://oauth.reddit.com/r/" + subreddit + "/comments/" + short_id;
        auto comments_resp = cpr::Get(cpr::Url{comments_url}, oauth_headers);

        std::string joined_comments;
        if (comments_resp.status_code == 200) {
            auto comments_json = crow::json::load(comments_resp.text);
            if (comments_json.t() == crow::json::type::List && comments_json.lo().size() > 1) {
                auto comment_listing = comments_json[1]["data"]["children"];
                if (comment_listing && comment_listing.t() == crow::json::type::List) {
                    std::vector<std::string> comment_bodies;
                    for (auto& c : comment_listing.lo()) {
                        if (!c.has("data")) continue;
                        auto c_data = c["data"];
                        if (c_data.has("body")) {
                            comment_bodies.push_back(c_data["body"].s());
                        }
                    }
                    for (size_t i = 0; i < comment_bodies.size(); i++) {
                        joined_comments += comment_bodies[i];
                        if (i + 1 < comment_bodies.size()) {
                            joined_comments += "|";
                        }
                    }
                }
            }
        }
        single_post["comments"] = joined_comments;
        
        posts_array.push_back(single_post);
    }
    return posts_array;
}

std::vector<crow::json::wvalue> Reddit::get_complaints_from_posts(std::vector<crow::json::wvalue> posts) {
    crow::json::wvalue request_body;
    request_body["posts"] = std::move(posts);

    std::string analytics_request_str = request_body.dump();
    auto analytics_resp = cpr::Post(
        cpr::Url{"http://host.docker.internal:8000/process_complaints"},
        cpr::Body{analytics_request_str},
        cpr::Header{{"Content-Type", "application/json"}}
    );

    if (analytics_resp.status_code != 200) {
        throw std::runtime_error("Failed to retrieve data from analytics. Status code: " + std::to_string(analytics_resp.status_code));
    }

    auto analytics_json = crow::json::load(analytics_resp.text);
    if (!analytics_json || !analytics_json.has("complaints")) {
        throw std::runtime_error("Analytics response missing 'complaints'");
    }

    auto complaints = analytics_json["complaints"];
    if (complaints.t() != crow::json::type::List) {
        throw std::runtime_error("'complaints' is not an array.");
    }

    std::vector<crow::json::wvalue> result;
    for (auto& complaint_rval: complaints.lo()) {
        crow::json::wvalue complaint_wval{complaint_rval};
        result.push_back(complaint_wval);
    }
    return result;
}