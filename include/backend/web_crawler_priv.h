#ifndef WEB_CRAWLER_PRIV_H
#define WEB_CRAWLER_PRIV_H

#include <curl/curl.h>
#include "web_crawler_config.h"

struct GetContentListHandle {
    size_t max_content_length;
    size_t max_num_responses;
    const char* env_path;
    CURL* (*create_curl_handle)();
    void (*load_env)(const char*);
    char* (*get_google_search_url)(const char*);
    char* (*fetch_webpage_content)(const char*, int*, CURL*, struct curl_slist*);
    char* (*structure_google_query_response)(const char*, size_t, size_t);
    void (*destroy_curl_handle)(CURL*);
};

struct GetContentItemHandle {
    size_t max_content_length;
    size_t max_num_comments;
    int min_score;
    const char* env_path;
    CURL* (*create_curl_handle)();
    struct curl_slist* (*create_curl_headers)();
    WebsiteType (*detect_website_type)(const char*);
    void (*load_env)(const char*);
    char** (*web_specific_setup)(const char *, WebsiteType, CURL*, struct curl_slist**, int*, size_t, size_t*);
    char* (*fetch_webpage_content)(const char*, int*, CURL*, struct curl_slist*);
    char* (*structure_webpage_content_response)(char**, size_t, WebsiteType, size_t, size_t, int);
    void (*destroy_curl_handle)(CURL*);
    void (*destroy_curl_headers)(struct curl_slist*);
};

#endif