#ifndef WEB_CRAWLER_H
#define WEB_CRAWLER_H

#define MAX_RESPONSE_TITLE_LEN 128
#define MAX_RESPONSE_LINK_LEN 256
#define MAX_RESPONSE_WEB_CONTENT_LEN 1024

typedef struct GetContentListHandle GetContentListHandle;
typedef struct GetContentItemHandle GetContentItemHandle;

GetContentListHandle* get_content_list_handle(size_t max_content_length, size_t max_num_responses);
GetContentItemHandle* get_content_item_handle(size_t max_content_length, size_t max_num_comments, int max_score);
char* get_content_list(const char* query, int* status_code, GetContentListHandle* handle);
char* get_content_item(const char* url, int* status_code, int* escaped, GetContentItemHandle* handle);

#endif