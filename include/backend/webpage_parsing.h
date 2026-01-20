#ifndef WEBPAGE_PARSING
#define WEBPAGE_PARSING

#include "content_formatting.h"

typedef enum {
    PARSE_WEBPAGE_CONTENT_OK = 0,
} ParseWebpageContentError;

ParseWebpageContentError parse_stackoverflow_content(const char* content, ContentList* content_list);

#endif