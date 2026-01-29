#ifndef WEBPAGE_PARSING
#define WEBPAGE_PARSING

#define MAX_CONTENT_ITEMS 5
#define MAX_CONTENT_BODY_LEN 4096

typedef enum {
    PARSE_WEBPAGE_CONTENT_BAD = -1,
    PARSE_WEBPAGE_CONTENT_OK = 0,
} ParseWebpageContentError;

typedef struct ContentItem {
    char content_body[MAX_CONTENT_BODY_LEN];
    int score;
} ContentItem;

typedef struct ContentList {
    ContentItem items[MAX_CONTENT_ITEMS];
    size_t num_items;
} ContentList;

ParseWebpageContentError parse_stackoverflow_content(char** content, size_t num_urls, ContentList* comments, ContentItem* original_post, size_t max_num_comments, int min_score);
ParseWebpageContentError parse_reddit_content(char** content, size_t num_urls, ContentList* comments, ContentItem* original_post, size_t max_num_comments, int min_score);

#endif