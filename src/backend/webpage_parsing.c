#include "webpage_parsing.h"
#include "json.h"
#include <stdlib.h>

#define NUM_BYTES_TO_READ_STACKOVERFLOW 32768
#define NUM_BYTES_TO_READ_REDDIT 65536

// REQUIRES: Content in the format of a stackoverflow json api response
// MODIFIES: content_list with parsed content
// EFFECTS: Parses through content string and fills content_list
// TODO: Implement other parsing functions and abstract things where neccessary
ParseWebpageContentError parse_stackoverflow_content(char* content, ContentList* content_list) {
    char* items = get_json_value(content, "items");
    if (!items) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }
    int num_elements = 0;
    char** items_array = separate_array(items, &num_elements, NUM_BYTES_TO_READ_STACKOVERFLOW);
    free(items);
    if (!items_array) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }

    for (int i = 0; i < num_elements && i < MAX_CONTENT_ITEMS; i++) {
        char* item_body = get_json_value(items_array[i], "body");

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

// REQUIRES: Reddit JSON response
// EFFECTS: Parses response and returns 
// TODO: Improve efficiency after improving json.c
static char** get_reddit_comments(char* content, int* num_elements) {
    *num_elements = 0;
    char** items_array = separate_array(content, num_elements, NUM_BYTES_TO_READ_REDDIT);
    if (!items_array || *num_elements < 2) {
        return NULL;
    }

    char* children = get_json_value(items_array[1], "children");
    for (int i = 0; i < *num_elements; i++) {
        free(items_array[i]);
    }
    free(items_array);
    if (!children) {
        return NULL;
    }

    *num_elements = 0;
    char** children_array = separate_array(children, num_elements, NUM_BYTES_TO_READ_REDDIT);
    free(children);

    return children_array;
}

ParseWebpageContentError parse_reddit_content(char* content, ContentList* content_list) {
    int num_elements = 0;
    char** comments = get_reddit_comments(content, &num_elements);
    if (!comments) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }

    for (int i = 0; i < num_elements && i < MAX_CONTENT_ITEMS; i++) {
        char* kind = get_json_value(comments[i], "kind");
        if (strcmp(kind, "\"t1\"")) {
            continue;
        }
        char* item_body = get_json_value(comments[i], "body_html");

        if (strlen(item_body) >= MAX_CONTENT_BODY_LEN) {
            continue;
        }
        strncpy(content_list->items[i].content_body, item_body, MAX_CONTENT_BODY_LEN - 1);
        char* score_string = get_json_value(comments[i], "score");
        content_list->items[i].score = atoi(score_string);

        content_list->num_items++;
        free(item_body);
    }

    for (int i = 0; i < num_elements; i++) {
        free(comments[i]);
    }
    free(comments);
    return PARSE_WEBPAGE_CONTENT_OK;
}