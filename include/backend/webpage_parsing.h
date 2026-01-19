#ifndef WEBPAGE_PARSING
#define WEBPAGE_PARSING

typedef struct ContentItem ContentItem;
typedef struct ContentList ContentList;

void parse_reddit_content(const char* html_content, ContentList* content_list);

#endif