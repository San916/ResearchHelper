#include "web_crawler_config.h"

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

// REQUIRES: Valid reddit post url, output string and length
// MODIFIES: output string
// EFFECTS: Write question id in out, returns 1 on success, 0 if fail
int extract_reddit_question_id(const char* url, char* out, size_t out_len) {
    const char* pattern = "comments/";
    char* first_slash = strstr(url, pattern);
    if (!first_slash) {
        return 0;
    }
    first_slash = first_slash + strlen(pattern);

    char* second_slash = strchr(first_slash, '/');
    if (!second_slash) {
        return 0;
    }

    size_t question_id_length = second_slash - first_slash;
    strncpy(out, first_slash, question_id_length);

    return 1;
}

// REQUIRES: url, curl handle and headers, reference to int flag, REDDIT_ID env should exist
// MODIFIES: curl handle and headers, escaped flag
// EFFECTS: Does reddit-specific setup, returns new url
char* setup_reddit_url(const char* url, CURL* curl_handle, struct curl_slist** headers, int* escaped, size_t max_num_comments) {
    char* new_url = calloc(1, MAX_CURL_URL_LEN);
    if (!new_url) {
        return NULL;
    }

    char question_id[32] = {0};
    if (!extract_reddit_question_id(url, question_id, 32)) {
        return NULL;
    }

    snprintf(new_url, MAX_CURL_URL_LEN, "https://www.reddit.com/comments/%s.json?limit=%zu&depth=%d&sort=top", question_id, max_num_comments, REDDIT_API_DEPTH);

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

// REQUIRES: Valid stackexchange post url, output string and length
// MODIFIES: output string
// EFFECTS: Write question id in out, returns 1 on success, 0 if fail
int extract_stackoverflow_question_id(const char* url, char* out, size_t out_len) {
    const char* pattern = "questions/";
    char* first_slash = strstr(url, pattern);
    if (!first_slash) {
        return 0;
    }
    first_slash = first_slash + strlen(pattern);

    char* second_slash = strchr(first_slash, '/');
    if (!second_slash) {
        return 0;
    }

    size_t question_id_length = second_slash - first_slash;
    strncpy(out, first_slash, question_id_length);
    
    return 1;
}

// REQUIRES: Valid stackexchange post url, output string and length
// MODIFIES: output string
// EFFECTS: Write site name in out, returns 1 on success, 0 if fail
int extract_stackoverflow_site(const char* url, char* out, size_t out_len) {
    const char* pattern = "://";
    char* start = strstr(url, pattern);
    if (!start) {
        return 0;
    }
    start = start + strlen(pattern);

    char* end = strchr(start, '/');
    if (!end) {
        return 0;
    }

    size_t site_len = end - start;
    char site[256];
    strncpy(site, start, site_len);
    site[site_len] = '\0';

    if (!strcmp(site, "stackoverflow.com")) {
        strncpy(out, "stackoverflow", out_len);
        return 1;
    } else if (!strcmp(site, "superuser.com")) {
        strncpy(out, "superuser", out_len);
        return 1;
    } else if (!strcmp(site, "serverfault.com")) {
        strncpy(out, "serverfault", out_len);
        return 1;
    }

    const char *suffix = ".stackexchange.com";
    size_t suffix_len = strlen(suffix);
    if (!strcmp(site + site_len - suffix_len, suffix)) {
        strncpy(out, site, site_len - suffix_len);
        return 1;
    }

    return 0;
}

// REQUIRES: url, curl handle and headers, reference to int flag
// MODIFIES: curl handle and headers, escaped flag
// EFFECTS: Does stackoverflow-specific setup, returns new url
char* setup_stackoverflow_url(const char* url, CURL* curl_handle, struct curl_slist** headers, int* escaped) {
    char* new_url = calloc(1, MAX_CURL_URL_LEN);
    if (!new_url) {
        return NULL;
    }

    printf("GETTING QUESTION ID\n");
    char question_id[32] = {0};
    if (!extract_stackoverflow_question_id(url, question_id, 32)) {
        return NULL;
    }
    printf("QUESTION ID: %s\n", question_id);

    char site[32] = {0};
    if (!extract_stackoverflow_site(url, site, 32)) {
        return NULL;
    }
    printf("SITE: %s\n", site);

    snprintf(new_url, MAX_CURL_URL_LEN, "https://api.stackexchange.com/2.3/questions/%s/answers?site=%s&order=desc&sort=votes&filter=withbody", question_id, site);

    *headers = curl_slist_append(*headers, "Accept: application/json");
    curl_easy_setopt(curl_handle, CURLOPT_URL, new_url);
    curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");

    printf("NEW_URL: %s\n", new_url);
    return new_url;
}

// REQUIRES: Valid url and WebsiteType, curl handler and headers, max num comments
// EFFECTS: Executes different curl_handle setups according to the website type, sets escaped flag if necessary, sets url to return max_num_comments if possible
// new url to curl (some websites may want us to append .json, add .api, etc)
char* web_specific_setup(const char* url, WebsiteType type, CURL* curl_handle, struct curl_slist** headers, int* escaped, size_t max_num_comments) {
    if (strlen(url) >= MAX_CURL_URL_LEN) {
        return NULL;
    }
    char* new_url = NULL;

    switch (type) {
        case WEBSITE_REDDIT: {
            new_url = setup_reddit_url(url, curl_handle, headers, escaped, max_num_comments);
            break;
        } case WEBSITE_STACKOVERFLOW: {
            new_url = setup_stackoverflow_url(url, curl_handle, headers, escaped);
            break;
        } case WEBSITE_GITHUB: {
            break;
        } default: {
            break;
        }
    }

    return new_url;
}