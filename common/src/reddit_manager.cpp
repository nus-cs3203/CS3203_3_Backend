#include "reddit_manager.hpp"

#include <cpr/cpr.h>

#include <string>

#include "crow.h"
#include "date_utils.hpp"

RedditManager::RedditManager(const std::string& reddit_api_id, const std::string& reddit_api_secret,
                             const std::string& reddit_username, const std::string& reddit_password,
                             const std::string& user_agent)
    : reddit_api_id(reddit_api_id),
      reddit_api_secret(reddit_api_secret),
      reddit_username(reddit_username),
      reddit_password(reddit_password),
      user_agent(user_agent) {}

std::shared_ptr<RedditManager> RedditManager::create_from_env(EnvManager env_manager) {
    auto REDDIT_API_ID = env_manager.read_env("REDDIT_API_ID", Constants::REDDIT_API_ID);
    auto REDDIT_API_SECRET =
        env_manager.read_env("REDDIT_API_SECRET", Constants::REDDIT_API_SECRET);
    auto REDDIT_USERNAME = env_manager.read_env("REDDIT_USERNAME", Constants::REDDIT_USERNAME);
    auto REDDIT_PASSWORD = env_manager.read_env("REDDIT_PASSWORD", Constants::REDDIT_PASSWORD);
    auto USER_AGENT = env_manager.read_env("USER_AGENT", Constants::USER_AGENT);

    return std::make_shared<RedditManager>(REDDIT_API_ID, REDDIT_API_SECRET, REDDIT_USERNAME,
                                           REDDIT_PASSWORD, USER_AGENT);
}

std::string RedditManager::_get_access_token() {
    cpr::Authentication auth{reddit_api_id, reddit_api_secret, cpr::AuthMode::BASIC};
    cpr::Payload token_payload{
        {"grant_type", "password"}, {"username", reddit_username}, {"password", reddit_password}};
    cpr::Header base_headers{{"User-Agent", user_agent}};

    auto token_response = cpr::Post(cpr::Url{"https://www.reddit.com/api/v1/access_token"}, auth,
                                    token_payload, base_headers);

    if (token_response.status_code != 200) {
        throw std::runtime_error("Failed to obtain Reddit OAuth token. Status: " +
                                 std::to_string(token_response.status_code));
    }

    auto token_json = crow::json::load(token_response.text);
    if (!token_json || !token_json.has("access_token")) {
        throw std::runtime_error("Reddit token response is missing 'access_token'");
    }

    std::string access_token = token_json["access_token"].s();
    return access_token;
}

std::string remove_non_utf8(const std::string& input) {
    std::string output;
    size_t i = 0;
    while (i < input.size()) {
        unsigned char c = input[i];
        size_t bytesInChar = 0;

        if (c <= 0x7F) {
            bytesInChar = 1;
        } else if (c >= 0xC2 && c <= 0xDF) {
            bytesInChar = 2;
        } else if (c >= 0xE0 && c <= 0xEF) {
            bytesInChar = 3;
        } else if (c >= 0xF0 && c <= 0xF4) {
            bytesInChar = 4;
        } else {
            ++i;
            continue;
        }

        if (i + bytesInChar > input.size()) {
            break;
        }

        bool valid = true;
        for (size_t j = 1; j < bytesInChar; j++) {
            if ((static_cast<unsigned char>(input[i + j]) & 0xC0) != 0x80) {
                valid = false;
                break;
            }
        }

        if (valid) {
            if (bytesInChar == 3) {
                unsigned char second = input[i + 1];
                if (c == 0xE0 && second < 0xA0)
                    valid = false;
                if (c == 0xED && second > 0x9F)
                    valid = false;
            }
            if (bytesInChar == 4) {
                unsigned char second = input[i + 1];
                if (c == 0xF0 && second < 0x90)
                    valid = false;
                if (c == 0xF4 && second > 0x8F)
                    valid = false;
            }
        }

        if (valid) {
            output.append(input, i, bytesInChar);
            i += bytesInChar;
        } else {
            ++i;
        }
    }
    return output;
}

std::vector<crow::json::wvalue> RedditManager::get_posts(const std::string& subreddit) {
    cpr::Header base_headers{{"User-Agent", user_agent}};
    cpr::Header oauth_headers = base_headers;
    std::string access_token = _get_access_token();
    oauth_headers["Authorization"] = "bearer " + access_token;

    auto listing_response = cpr::Get(
        cpr::Url{"https://oauth.reddit.com/r/" + subreddit + "/new?limit=10"}, oauth_headers);

    if (listing_response.status_code != 200) {
        throw std::runtime_error(
            "Failed to retrieve data from /r/" + subreddit +
            "/new. Status code: " + std::to_string(listing_response.status_code));
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

    for (auto& child : children.lo()) {
        if (!child.has("data"))
            continue;
        auto post_data = child["data"];
        if (!post_data)
            continue;

        crow::json::wvalue single_post;

        std::vector<std::string> double_fields = {"upvote_ratio"};

        std::vector<std::string> int_fields = {"downs", "likes", "num_comments",
                                               "score", "ups",   "view_count"};

        std::vector<std::string> string_fields = {"author_flair_text", "selftext", "title", "url",
                                                  "id"};

        for (const auto& field : double_fields) {
            if (post_data[field].t() == crow::json::type::Null) {
                single_post[field] = post_data[field];
                continue;
            }

            single_post[field] = post_data[field].d();
        }

        for (const auto& field : int_fields) {
            if (post_data[field].t() == crow::json::type::Null) {
                single_post[field] = post_data[field];
                continue;
            }

            single_post[field] = post_data[field].i();
        }

        for (const auto& field : string_fields) {
            if (post_data[field].t() == crow::json::type::Null) {
                single_post[field] = post_data[field];
                continue;
            }

            auto value = remove_non_utf8(static_cast<std::string>(post_data[field].s()));
            single_post[field] = "value";
        }
        single_post["date"] = DateUtils::utc_unix_timestamp_to_string(post_data["created"].i(),
                                                                      Constants::DATETIME_FORMAT);
        single_post["source"] = "Reddit";
        single_post["sub_source"] = subreddit;

        std::string short_id = post_data["id"].s();

        std::string comments_url =
            "https://oauth.reddit.com/r/" + subreddit + "/comments/" + short_id;
        auto comments_resp = cpr::Get(cpr::Url{comments_url}, oauth_headers);

        std::string joined_comments = "";
        try {
            auto comments_json = crow::json::load(comments_resp.text);
            auto comment_listing = comments_json[1]["data"]["children"];
            for (auto& comment : comment_listing.lo()) {
                joined_comments += static_cast<std::string>(comment["data"]["body"].s()) + "|";
            }
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        single_post["comments"] = remove_non_utf8(joined_comments);

        posts_array.push_back(std::move(single_post));
    }

    return posts_array;
}