#include "webpage_parsing.h"
#include "json.h"
#include <stdlib.h>

// REQUIRES: Reddit JSON response, max number of comments
// EFFECTS: Parses response and returns up to max_num_comments comments
// TODO: Improve efficiency after improving json.c
static char** get_reddit_comments(char* content, size_t* num_elements, size_t max_num_comments) {
    *num_elements = 0;
    char** items_array = separate_array(content, num_elements, 2);
    if (!items_array || *num_elements < 2) {
        return NULL;
    }

    char* children = get_json_value(items_array[1], "children");
    for (size_t i = 0; i < *num_elements; i++) {
        free(items_array[i]);
    }
    free(items_array);
    if (!children) {
        return NULL;
    }

    *num_elements = 0;
    char** children_array = separate_array(children, num_elements, MAX_CONTENT_ITEMS);
    free(children);

    return children_array;
}

// REQUIRES: Content in the format of a Reddit json api response (assumes response is sorted with score), max comments to return, min score
// MODIFIES: content_list with parsed content
// EFFECTS: Parses through content string and fills content_list up to max_num_comments or MAX_CONTENT_ITEMS
// TODO: Implement other parsing functions and abstract things where neccessary
ParseWebpageContentError parse_reddit_content(char* content, ContentList* content_list, size_t max_num_comments, int min_score) {
    max_num_comments = max_num_comments > MAX_CONTENT_ITEMS ? MAX_CONTENT_ITEMS : max_num_comments;
    size_t num_elements = 0;
    char** comments = get_reddit_comments(content, &num_elements, max_num_comments);
    if (!comments) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }

    for (size_t i = 0; i < num_elements && i < max_num_comments; i++) {
        char* kind = get_json_value(comments[i], "kind");
        if (strcmp(kind, "\"t1\"")) {
            continue;
        }

        char* score_string = get_json_value(comments[i], "score");
        int score_int = atoi(score_string);
        if (score_int < min_score) {
            break;
        }
        content_list->items[i].score = score_int;

        char* item_body = get_json_value(comments[i], "body_html");
        if (strlen(item_body) >= MAX_CONTENT_BODY_LEN) {
            continue;
        }
        strncpy(content_list->items[i].content_body, item_body, MAX_CONTENT_BODY_LEN - 1);

        content_list->num_items++;
        free(item_body);
    }

    for (size_t i = 0; i < num_elements; i++) {
        free(comments[i]);
    }
    free(comments);
    return PARSE_WEBPAGE_CONTENT_OK;
}

// REQUIRES: Content in the format of a stackoverflow json api response (assumes response is sorted with score), max comments to return, min score
// MODIFIES: content_list with parsed content
// EFFECTS: Parses through content string and fills content_list up to max_num_comments or MAX_CONTENT_ITEMS
// TODO: Implement other parsing functions and abstract things where neccessary
ParseWebpageContentError parse_stackoverflow_content(char* content, ContentList* content_list, size_t max_num_comments, int min_score) {
    max_num_comments = max_num_comments > MAX_CONTENT_ITEMS ? MAX_CONTENT_ITEMS : max_num_comments;
    char* items = get_json_value(content, "items");
    if (!items) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }
    size_t num_elements = 0;
    char** comments = separate_array(items, &num_elements, max_num_comments);
    free(items);
    if (!comments) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }

    for (size_t i = 0; i < num_elements && i < max_num_comments; i++) {
        char* score_string = get_json_value(comments[i], "score");
        int score_int = atoi(score_string);
        if (score_int < min_score) {
            break;
        }
        content_list->items[i].score = score_int;

        char* item_body = get_json_value(comments[i], "body");
        if (strlen(item_body) >= MAX_CONTENT_BODY_LEN) {
            continue;
        }
        strncpy(content_list->items[i].content_body, item_body, MAX_CONTENT_BODY_LEN - 1);

        content_list->num_items++;
        free(item_body);
    }

    for (size_t i = 0; i < num_elements; i++) {
        free(comments[i]);
    }
    free(comments);
    return PARSE_WEBPAGE_CONTENT_OK;
}