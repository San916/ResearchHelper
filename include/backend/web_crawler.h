#ifndef WEB_CRAWLER_H
#define WEB_CRAWLER_H

#define MAX_RESPONSE_TITLE_LEN 128
#define MAX_RESPONSE_LINK_LEN 256
#define MAX_RESPONSE_WEB_CONTENT_LEN 1024

char* get_content_list(const char* query, int* status_code, size_t max_content_length, size_t max_num_responses);
char* get_content_item(const char* url, int* status_code, int* escaped, size_t max_content_length, size_t max_num_comments);

#endif