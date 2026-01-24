#include "web_crawler_config.h"
#include "web_utils.h"

// REQUIRES: Valud url
// EFFECTS: Returns the specific type of website in the url
WebsiteType detect_website_type(const char* url) {
    if (strstr(url, "reddit.com")) return WEBSITE_REDDIT;
    if (strstr(url, "stackoverflow.com") || strstr(url, "stackexchange.com")) return WEBSITE_STACKOVERFLOW;
    if (strstr(url, "github.com")) return WEBSITE_GITHUB;
    return WEBSITE_UNKNOWN;
}

// REQUIRES: URL encoded input query, GOOGLE_SEARCH_API_KEY and GOOGLE_SEARCH_ENGINE envs should exist
// EFFECTS: Returns input query in google search api format
char* get_google_search_url(const char* input) {
    char* api_key = getenv("GOOGLE_SEARCH_API_KEY");
    char* search_engine_id = getenv("GOOGLE_SEARCH_ENGINE");
    if (!api_key || !search_engine_id) {
        return NULL;
    }
    char* url = calloc(1, MAX_CURL_URL_LEN);
    if (!url) return NULL;
    snprintf(url, MAX_CURL_URL_LEN, "https://www.googleapis.com/customsearch/v1?key=%s&cx=%s&q=%s", api_key, search_engine_id, input);
    return url;
}

// REQUIRES: url, curl handle and headers, reference to int flag, REDDIT_ID env should exist
// MODIFIES: curl handle and headers, escaped flag
// EFFECTS: Does reddit-specific setup, returns new url
char* setup_reddit_url(const char* url, CURL* curl_handle, struct curl_slist** headers, int* escaped) {
    char* new_url = calloc(1, MAX_CURL_URL_LEN);
    if (!new_url) {
        return NULL;
    }

    char* question_id = extract_reddit_question_id(url);
    if (!question_id) {
        return NULL;
    }

    snprintf(new_url, MAX_CURL_URL_LEN, "https://www.reddit.com/comments/%s.json?limit=%d&depth=%d&sort=top", question_id, REDDIT_API_LIMIT, REDDIT_API_DEPTH);
    free(question_id);

    char* reddit_id = getenv("REDDIT_ID");
    if (!reddit_id) {
        return NULL;
    }

    char user_agent[MAX_USER_AGENT_LEN] = {0};
    snprintf(user_agent, MAX_USER_AGENT_LEN, "windows:ResearchHelper:v1.0 (by %s)", reddit_id);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, user_agent);
    curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl_handle, CURLOPT_URL, new_url);

    *escaped = 1;
    return new_url;
}

// REQUIRES: url, curl handle and headers, reference to int flag
// MODIFIES: curl handle and headers, escaped flag
// EFFECTS: Does stackoverflow-specific setup, returns new url
char* setup_stackoverflow_url(const char* url, CURL* curl_handle, struct curl_slist** headers, int* escaped) {
    char* new_url = calloc(1, MAX_CURL_URL_LEN);
    if (!new_url) {
        return NULL;
    }

    char* question_id = extract_stackoverflow_question_id(url);
    if (!question_id) {
        return NULL;
    }

    snprintf(new_url, MAX_CURL_URL_LEN, "https://api.stackexchange.com/2.3/questions/%s/answers?site=stackoverflow&filter=withbody", question_id);
    free(question_id);

    *headers = curl_slist_append(*headers, "Accept: application/json");
    curl_easy_setopt(curl_handle, CURLOPT_URL, new_url);
    curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");

    return new_url;
}

// REQUIRES: Valid url and WebsiteType
// EFFECTS: Executes different curl_handle setups according to the website type
// new url to curl (some websites may want us to append .json, add .api, etc)
char* web_specific_setup(const char* url, WebsiteType type, CURL* curl_handle, struct curl_slist** headers, int* escaped) {
    char* new_url = NULL;

    switch (type) {
        case WEBSITE_REDDIT: {
            new_url = setup_reddit_url(url, curl_handle, headers, escaped);
        } case WEBSITE_STACKOVERFLOW: {
            new_url = setup_reddit_url(url, curl_handle, headers, escaped);
        } case WEBSITE_GITHUB: {
        } default: {
        }
    }

    
    return new_url;
}