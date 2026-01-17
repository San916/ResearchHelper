#ifndef WEB_CRAWLER_H
#define WEB_CRAWLER_H

#define MAX_NUM_RESPONSES 5
#define MAX_RESPONSE_TITLE_LEN 128
#define MAX_RESPONSE_LINK_LEN 256
#define MAX_RESPONSE_WEB_CONTENT_LEN 1024
#define MAX_CONTENT_ITEMS 5
#define MAX_CODE_LEN 512
#define MAX_DISCUSSION_LEN 512
#define MAX_CURL_URL_LEN 512

typedef struct WritebackData {
    char* data;
    size_t size;
} WritebackData;

typedef struct SingleResponse {
    char link[MAX_RESPONSE_LINK_LEN];
    char title[MAX_RESPONSE_TITLE_LEN];
    char web_content[MAX_RESPONSE_WEB_CONTENT_LEN];
} SingleResponse;

typedef struct QueryResponse {
    SingleResponse responses[MAX_NUM_RESPONSES];
    int num_responses;
} QueryResponse;

typedef struct ContentItem {
    char code[MAX_CODE_LEN];
    char discussion[MAX_DISCUSSION_LEN];
    int score;
} ContentItem;

typedef struct ContentList {
    ContentItem items[MAX_CONTENT_ITEMS];
    int num_items;
} ContentList;

char* get_google_search_url(char* input);
QueryResponse* structure_query_response(const char* input);
char* fetch_webpage_content(const char* url, int* status_code);
ContentList* parse_webpage_content(const char* html_content, const char* url, int* status_code);

#endif