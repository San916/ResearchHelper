#ifndef WEB_UTILS_H
#define WEB_UTILS_H

#include <stdlib.h>
#include <curl/curl.h>

#define MAX_CURL_URL_LEN 512
#define MAX_USER_AGENT_LEN 512

#define REDDIT_API_DEPTH 1
#define REDDIT_API_LIMIT 5

typedef struct WritebackData {
    char* data;
    size_t size;
} WritebackData;

typedef enum {
    WEBSITE_UNKNOWN,
    WEBSITE_REDDIT,
    WEBSITE_STACKOVERFLOW,
    WEBSITE_GITHUB,
    WEBSITE_STUB,
} WebsiteType;

void load_env(const char *file_name);
size_t write_memory_callback(char* content, size_t size, size_t nmemb, void* data_ptr);
CURL* create_curl_handle();
struct curl_slist* create_curl_headers();
void destroy_curl_handle(CURL* curl_handle);
void destroy_curl_headers(struct curl_slist* headers);
char* fetch_webpage_content(const char* url, int* status_code, CURL* curl_handle, struct curl_slist* headers);

char* get_google_search_url(const char* input);
WebsiteType detect_website_type(const char* url);
char* web_specific_setup(const char* url, WebsiteType type, CURL* curl_handle, struct curl_slist** headers);

#endif