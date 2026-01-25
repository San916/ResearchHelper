#ifndef CONTENT_FORMATTING_H
#define CONTENT_FORMATTING_H

#include "web_crawler_config.h"
#include "webpage_parsing.h"

#define MAX_NUM_RESPONSES 10
#define MAX_RESPONSE_TITLE_LEN 128
#define MAX_RESPONSE_LINK_LEN 256
#define MAX_RESPONSE_WEB_CONTENT_LEN 1024

typedef struct SingleResponse {
    char link[MAX_RESPONSE_LINK_LEN];
    char title[MAX_RESPONSE_TITLE_LEN];
    char web_content[MAX_RESPONSE_WEB_CONTENT_LEN];
} SingleResponse;

typedef struct QueryResponse {
    SingleResponse responses[MAX_NUM_RESPONSES];
    int num_responses;
} QueryResponse;

QueryResponse* parse_google_query_response(const char* input, int max_num_responses);
char* stringify_google_query_response(QueryResponse* query_response, size_t max_length);
char* structure_google_query_response(const char* content, size_t max_length, int max_num_responses);

ContentList* parse_webpage_content(char* content, WebsiteType website_type);
char* stringify_content_response(ContentList* content, size_t max_length);
char* structure_webpage_content_response(char* content, WebsiteType website_type, size_t max_length);

#endif