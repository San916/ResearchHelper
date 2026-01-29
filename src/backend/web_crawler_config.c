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
// EFFECTS: Write question id in out, returns 0 on success, -1 if fail
int extract_reddit_question_id(const char* url, char* out, size_t out_len) {
    const char* pattern = "comments/";
    char* first_slash = strstr(url, pattern);
    if (!first_slash) {
        return -1;
    }
    first_slash = first_slash + strlen(pattern);

    char* second_slash = strchr(first_slash, '/');
    if (!second_slash) {
        return -1;
    }

    size_t question_id_length = second_slash - first_slash;
    strncpy(out, first_slash, question_id_length);

    return 0;
}

// REQUIRES: url, curl handle and headers, reference to int flag, REDDIT_ID env should exist
// MODIFIES: curl handle and headers, escaped flag
// EFFECTS: Does reddit-specific setup, returns new url
char* setup_reddit_url(const char* url, CURL* curl_handle, struct curl_slist** headers, int* escaped, size_t max_num_comments, size_t* num_urls) {
    char* new_url = calloc(1, MAX_CURL_URL_LEN);
    if (!new_url) {
        return NULL;
    }

    char question_id[32] = {0};
    if (extract_reddit_question_id(url, question_id, 32)) {
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
    *num_urls = 1;
    return new_url;
}

// REQUIRES: Valid stackexchange post url, output string and length
// MODIFIES: output string
// EFFECTS: Write question id in out, returns 0 on success, -1 if fail
int extract_stackoverflow_question_id(const char* url, char* out, size_t out_len) {
    const char* pattern = "questions/";
    char* first_slash = strstr(url, pattern);
    if (!first_slash) {
        return -1;
    }
    first_slash = first_slash + strlen(pattern);

    char* second_slash = strchr(first_slash, '/');
    if (!second_slash) {
        return -1;
    }

    size_t question_id_length = second_slash - first_slash;
    strncpy(out, first_slash, question_id_length);
    
    return 0;
}

// REQUIRES: Valid stackexchange post url, output string and length
// MODIFIES: output string
// EFFECTS: Write site name in out, returns 0 on success, -1 if fail
int extract_stackoverflow_site(const char* url, char* out, size_t out_len) {
    const char* pattern = "://";
    char* start = strstr(url, pattern);
    if (!start) {
        return -1;
    }
    start = start + strlen(pattern);

    char* end = strchr(start, '/');
    if (!end) {
        return -1;
    }

    size_t site_len = end - start;
    char site[256];
    strncpy(site, start, site_len);
    site[site_len] = '\0';

    if (!strcmp(site, "stackoverflow.com")) {
        strncpy(out, "stackoverflow", out_len);
        return 0;
    } else if (!strcmp(site, "superuser.com")) {
        strncpy(out, "superuser", out_len);
        return 0;
    } else if (!strcmp(site, "serverfault.com")) {
        strncpy(out, "serverfault", out_len);
        return 0;
    }

    const char *suffix = ".stackexchange.com";
    size_t suffix_len = strlen(suffix);
    if (!strcmp(site + site_len - suffix_len, suffix)) {
        strncpy(out, site, site_len - suffix_len);
        return 0;
    }

    return -1;
}

// REQUIRES: url, curl handle and headers, reference to int flag
// MODIFIES: curl handle and headers, escaped flag
// EFFECTS: Does stackoverflow-specific setup, returns url to original post and url to answers to post
char** setup_stackoverflow_urls(const char* url, CURL* curl_handle, struct curl_slist** headers, int* escaped, size_t* num_urls) {
    char** new_urls = malloc(2 * sizeof(char*));
    if (!new_urls) {
        return NULL;
    }
    new_urls[0] = calloc(1, MAX_CURL_URL_LEN);
    new_urls[1] = calloc(1, MAX_CURL_URL_LEN);
    if (!new_urls[0] || !new_urls[1]) {
        if (new_urls[0]) free(new_urls[0]);
        if (new_urls[1]) free(new_urls[1]);
        free(new_urls);
        return NULL;
    }

    char question_id[32] = {0};
    if (extract_stackoverflow_question_id(url, question_id, 32)) {
        return NULL;
    }

    char site[32] = {0};
    if (extract_stackoverflow_site(url, site, 32)) {
        return NULL;
    }

    snprintf(new_urls[0], MAX_CURL_URL_LEN, "https://api.stackexchange.com/2.3/questions/%s/?site=%s&filter=withbody", question_id, site);
    snprintf(new_urls[1], MAX_CURL_URL_LEN, "https://api.stackexchange.com/2.3/questions/%s/answers?site=%s&order=desc&sort=votes&filter=withbody", question_id, site);

    *headers = curl_slist_append(*headers, "Accept: application/json");
    curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");

    *num_urls = 2;
    *escaped = 0;
    return new_urls;
}

// REQUIRES: Valid url and WebsiteType, curl handler and headers, max num comments, reference to num_urls
// MODIFIES: sets num_urls
// EFFECTS: Executes different curl_handle setups according to the website type, sets escaped flag if necessary, sets url to return max_num_comments if possible
// new urls to curl (some websites may want us to append .json, add .api, etc) and number of urls
char** web_specific_setup(const char* url, WebsiteType type, CURL* curl_handle, struct curl_slist** headers, int* escaped, size_t max_num_comments, size_t* num_urls) {
    if (strlen(url) >= MAX_CURL_URL_LEN) {
        return NULL;
    }
    char** new_urls = NULL;

    switch (type) {
        case WEBSITE_REDDIT: {
            new_urls = malloc(1 * sizeof(char*));
            new_urls[0] = setup_reddit_url(url, curl_handle, headers, escaped, max_num_comments, num_urls);
            break;
        } case WEBSITE_STACKOVERFLOW: {
            new_urls = setup_stackoverflow_urls(url, curl_handle, headers, escaped, num_urls);
            break;
        } case WEBSITE_GITHUB: {
            break;
        } default: {
            break;
        }
    }

    return new_urls;
}