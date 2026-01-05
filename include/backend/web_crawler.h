#ifndef WEB_CRAWLER_H
#define WEB_CRAWLER_H

#define MAX_NUM_RESPONSES 5
#define MAX_RESPONSE_TITLE_LEN 128
#define MAX_RESPONSE_URL_LEN 256
#define MAX_RESPONSE_WEB_CONTENT_LEN 1024

typedef struct SingleResponse {
    char url[MAX_RESPONSE_URL_LEN];
    char title[MAX_RESPONSE_TITLE_LEN];
    char web_content[MAX_RESPONSE_WEB_CONTENT_LEN];
} SingleResponse;

typedef struct QueryResponse {
    SingleResponse responses[1];
} QueryResponse;

size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp);
QueryResponse* input_query(char* input, int* status_code);

#endif