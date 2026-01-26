#ifndef WEB_CRAWLER_CONFIG_H
#define WEB_CRAWLER_CONFIG_H

#include <curl/curl.h>

#define MAX_CURL_URL_LEN 512
#define MAX_USER_AGENT_LEN 512

#define REDDIT_API_DEPTH 1

typedef enum {
    WEBSITE_UNKNOWN,
    WEBSITE_REDDIT,
    WEBSITE_STACKOVERFLOW,
    WEBSITE_GITHUB,
    WEBSITE_STUB,
} WebsiteType;

WebsiteType detect_website_type(const char* url);
char* get_google_search_url(const char* input);
char* extract_reddit_question_id(const char* url);
char* extract_stackoverflow_question_id(const char* url);
char* setup_reddit_url(const char* url, CURL* curl_handle, struct curl_slist** headers, int* escaped, size_t max_num_comments);
char* setup_stackoverflow_url(const char* url, CURL* curl_handle, struct curl_slist** headers, int* escaped);
char* web_specific_setup(const char* url, WebsiteType type, CURL* curl_handle, struct curl_slist** headers, int* escaped, size_t max_num_comments);

#endif