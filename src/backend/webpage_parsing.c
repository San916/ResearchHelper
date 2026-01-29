#include "webpage_parsing.h"
#include "json.h"
#include <stdlib.h>
#include <stdio.h>

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
    char** children_array = separate_array(children, num_elements, max_num_comments);
    free(children);

    return children_array;
}

// REQUIRES: Content in the format of a Reddit json api response (assumes response is sorted with score), max comments to return, min score
// content[0] should contain both the original post and comments
// MODIFIES: content_list with parsed content, original_post with original post
// EFFECTS: Parses through content string and fills content_list with the minimum of MAX_CONTENT_ITEMS and max_num_comments
// TODO: Implement other parsing functions and abstract things where neccessary
ParseWebpageContentError parse_reddit_content(char** content, size_t num_urls, ContentList* comments, ContentItem* original_post, size_t max_num_comments, int min_score) {
    if (!content || num_urls != 1) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }
    max_num_comments = max_num_comments > MAX_CONTENT_ITEMS ? MAX_CONTENT_ITEMS : max_num_comments;

    char* original_post_string = get_json_value(content[0], "selftext_html");
    if (!original_post_string) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }
    strncpy(original_post->content_body, original_post_string, MAX_CONTENT_BODY_LEN - 1);
    free(original_post_string);

    size_t num_elements = 0;
    char** string_comments = get_reddit_comments(content[0], &num_elements, max_num_comments);
    if (!comments) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }

    for (size_t i = 0; i < num_elements; i++) {
        char* kind = get_json_value(string_comments[i], "kind");
        if (strcmp(kind, "\"t1\"")) {
            continue;
        }

        char* score_string = get_json_value(string_comments[i], "score");
        int score_int = atoi(score_string);
        if (score_int < min_score) {
            break;
        }
        comments->items[i].score = score_int;

        char* item_body = get_json_value(string_comments[i], "body_html");
        if (strlen(item_body) >= MAX_CONTENT_BODY_LEN) {
            continue;
        }
        strncpy(comments->items[i].content_body, item_body, MAX_CONTENT_BODY_LEN - 1);

        comments->num_items++;
        free(item_body);
        free(string_comments[i]);
    }
    free(string_comments);

    return PARSE_WEBPAGE_CONTENT_OK;
}

// REQUIRES: Content in the format of a Stackexchange json api response (assumes response is sorted with score), max comments to return, min score
// content[0] should contain the original post, content[1] the comments
// MODIFIES: content_list with parsed content, original_post with original post
// EFFECTS: Parses through content string and fills content_list with the minimum of MAX_CONTENT_ITEMS and max_num_comments
// TODO: Implement other parsing functions and abstract things where neccessary
ParseWebpageContentError parse_stackoverflow_content(char** content, size_t num_urls, ContentList* comments, ContentItem* original_post, size_t max_num_comments, int min_score) {
    if (!content || num_urls != 2) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }
    max_num_comments = max_num_comments > MAX_CONTENT_ITEMS ? MAX_CONTENT_ITEMS : max_num_comments;

    char* original_post_string = get_json_value(content[0], "body");
    if (!original_post_string) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }
    strncpy(original_post->content_body, original_post_string, MAX_CONTENT_BODY_LEN - 1);
    free(original_post_string);

    char* items = get_json_value(content[1], "items");
    if (!items) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }

    size_t num_elements = 0;
    char** string_comments = separate_array(items, &num_elements, max_num_comments);
    free(items);
    if (!string_comments) {
        return PARSE_WEBPAGE_CONTENT_BAD;
    }

    for (size_t i = 0; i < num_elements; i++) {
        char* score_string = get_json_value(string_comments[i], "score");
        int score_int = atoi(score_string);
        if (score_int < min_score) {
            break;
        }
        comments->items[i].score = score_int;

        char* item_body = get_json_value(string_comments[i], "body");
        if (strlen(item_body) >= MAX_CONTENT_BODY_LEN) {
            continue;
        }
        strncpy(comments->items[i].content_body, item_body, MAX_CONTENT_BODY_LEN - 1);

        comments->num_items++;
        free(item_body);
        free(string_comments[i]);
    }
    free(string_comments);

    return PARSE_WEBPAGE_CONTENT_OK;
}