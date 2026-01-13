#ifndef WEB_CRAWLER_H
#define WEB_CRAWLER_H

#define MAX_NUM_RESPONSES 5
#define MAX_RESPONSE_TITLE_LEN 128
#define MAX_RESPONSE_LINK_LEN 256
#define MAX_RESPONSE_WEB_CONTENT_LEN 1024

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

size_t write_memory_callback(void *content, size_t size, size_t nmemb, void *data_ptr);
QueryResponse* structure_query_response(const char* input);
QueryResponse* input_query(char* input, int* status_code);

#endif