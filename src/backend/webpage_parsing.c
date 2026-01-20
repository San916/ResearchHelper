#include "webpage_parsing.h"
#include "web_crawler.h"
#include "json.h"

#define NUM_BYTES_TO_READ_STACKOVERFLOW 16384

// REQUIRES: Content in the format of a stackoverflow json api response
// MODIFIES: content_list with parsed content
// EFFECTS: Parses through content string and fills content_list
// TODO: Implement other parsing functions and abstract things where neccessary
int parse_stackoverflow_content(const char* content, ContentList* content_list) {
    char* items = get_json_value(content, "items");
    if (!items) {
        return -1;
    }
    int num_elements = 0;
    char** items_array = separate_array(items, &num_elements, NUM_BYTES_TO_READ_STACKOVERFLOW);
    free(items);
    if (!items_array) {
        return -1;
    }

    int max_num_elements = sizeof(content_list->items) / sizeof(ContentItem);
    for (int i = 0; i < num_elements && i < max_num_elements; i++) {
        char* item_body = get_json_value(items_array[i], "body");
        printf("ITEM[%d]_BODY: %s\n", i, item_body);

        if (strlen(item_body) >= MAX_CONTENT_BODY_LEN) {
            continue;
        }
        strncpy(content_list->items[i].content_body, item_body, MAX_CONTENT_BODY_LEN - 1);
        char* score_string = get_json_value(items_array[i], "score");
        content_list->items[i].score = atoi(score_string);

        content_list->num_items++;
        free(item_body);
    }

    for (int i = 0; i < num_elements; i++) {
        free(items_array[i]);
    }
    free(items_array);
    return PARSE_WEBPAGE_CONTENT_OK;
}