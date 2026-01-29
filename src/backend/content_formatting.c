#include "content_formatting.h"

#include "json.h"

#include <stdlib.h>

// REQUIRES: Google search api response in JSON format with key "items"
// EFFECTS: Structures query response items and returns
QueryResponse* parse_google_query_response(const char* input, size_t max_num_responses) {
    if (max_num_responses < 1) {
        return NULL;
    }
    max_num_responses = (max_num_responses > MAX_NUM_RESPONSES) ? MAX_NUM_RESPONSES : max_num_responses;
    QueryResponse* response = malloc(sizeof(QueryResponse));
    if (!response) {
        return NULL;
    }

    size_t num_elems = 0;
    char* items = get_json_value(input, "items");
    if (!items) {
        free(response);
        return NULL;
    }
    char** items_array = separate_array(items, &num_elems, max_num_responses);
    if (!items_array) {
        free(items);
        free(response);
        return NULL;
    }

    response->num_responses = 0;
    for (size_t i = 0; i < num_elems; i++) {
        char* link = get_json_value(items_array[i], "link");
        char* title = get_json_value(items_array[i], "title");
        struct SingleResponse current_response = {0};
        strncpy(current_response.link, link, MAX_RESPONSE_LINK_LEN - 1);
        strncpy(current_response.title, title, MAX_RESPONSE_TITLE_LEN - 1);
        free(link);
        free(title);
        response->responses[i] = current_response;
        response->num_responses++;
    }

    free(items);
    for (size_t i = 0; i < num_elems; i++) {
        free(items_array[i]);
    }
    free(items_array);
    return response;
}

// REQUIRES: Structured google search query response, max response length
// EFFECTS: Turns the query response struct into a JSON string
char* stringify_google_query_response(QueryResponse* query_response, size_t max_length) {
    char* response_msg = calloc(1, max_length);
    if (!response_msg) {
        return NULL;
    }
    
    size_t current_position = 0;
    size_t remaining = max_length;
    current_position += snprintf(response_msg + current_position, remaining - current_position, "{\"results\":[");
    
    for (size_t i = 0 ; i < query_response->num_responses; i++) {
        if (i > 0) {
            current_position += snprintf(response_msg + current_position, remaining - current_position, ",");
        }
        current_position += snprintf(response_msg + current_position, remaining - current_position, 
            "{\"title\":%s,\"link\":%s}", 
            query_response->responses[i].title,
            query_response->responses[i].link);
        
        if (current_position >= remaining - 20) {
            break;
        }
    }

    current_position += snprintf(response_msg + current_position, remaining - current_position, "]}");
    if (current_position >= remaining) {
        free(response_msg);
        return NULL;
    }
    response_msg[current_position] = '\0';

    return response_msg;
}

// REQUIRES: Google search api response as a string, max length of string, max number of responses to return
// EFFECTS: Creates a string containing relevant information from response, in JSON format
char* structure_google_query_response(const char* content, size_t max_length, size_t max_num_responses) {
    QueryResponse* query_response = parse_google_query_response(content, max_num_responses);
    if (!query_response) {
        return NULL;
    }
    char* response_msg = stringify_google_query_response(query_response, max_length);
    free(query_response);
    if (!response_msg) {
        return NULL;
    }

    return response_msg;
}

// REQUIRES: Webpage content, website type
// MODIFIES: comments, original_post
// EFFECTS: Structures content into comments and original_post, returns 0 on success, -1 on fail
int parse_webpage_content(char** content, size_t num_urls, WebsiteType website_type, ContentList* comments, ContentItem* original_post, size_t max_num_comments, int min_score) {
    max_num_comments = (max_num_comments > MAX_CONTENT_ITEMS) ? MAX_CONTENT_ITEMS : max_num_comments;

    switch (website_type) {
        case WEBSITE_REDDIT: {
            int status_code = parse_reddit_content(content, num_urls, comments, original_post, max_num_comments, min_score);
            if (status_code) {
                return -1;
            }
            break;
        } case WEBSITE_STACKOVERFLOW: {
            int status_code = parse_stackoverflow_content(content, num_urls, comments, original_post, max_num_comments, min_score);
            if (status_code) {
                return -1;
            }
            break;
        } case WEBSITE_GITHUB: {
            break;
        } case WEBSITE_STUB: {
            // Return a stub
            strcpy(original_post->content_body, "\"what is i equal to?\"");
            comments->num_items++;
            strcpy(comments->items[0].content_body, "\"<code>int i = 0;</code>\"");
            comments->items[0].score = 10;
            break;
        } default: {
            break;
        }
    }

    return 0;
}

// REQUIRES: Structured webpage content, max response length
// content[0] should contain the original post (if it exists, other)
// EFFECTS: Turns content struct into a JSON string
char* stringify_content_response(ContentList* comments, ContentItem* original_post, size_t max_length) {
    char* response_msg = calloc(1, max_length);
    if (!response_msg) {
        return NULL;
    }

    size_t current_position = 0;
    size_t remaining = max_length;
    current_position += snprintf(response_msg + current_position, remaining - current_position, "{\"original_post\":");\
    current_position += snprintf(response_msg + current_position, remaining - current_position, 
        "{\"content_body\":%s,\"score\":%d}", 
        original_post->content_body,
        original_post->score
    );
    current_position += snprintf(response_msg + current_position, remaining - current_position, ",\"comments\":[");

    for (size_t i = 0 ; i < comments->num_items; i++) {
        if (i > 0) {
            current_position += snprintf(response_msg + current_position, remaining - current_position, ",");
        }
        current_position += snprintf(response_msg + current_position, remaining - current_position, 
            "{\"content_body\":%s,\"score\":%d}", 
            comments->items[i].content_body,
            comments->items[i].score
        );
        
        if (current_position >= remaining - 20) {
            break;
        }
    }

    current_position += snprintf(response_msg + current_position, remaining - current_position, 
        "],\"comment_count\":%zu}",
        comments->num_items
    );
    if (current_position >= remaining) {
        free(response_msg);
        return NULL;
    }

    response_msg[current_position] = '\0';

    return response_msg;
}

char* structure_webpage_content_response(char** content, size_t num_urls, WebsiteType website_type, size_t max_length, size_t max_num_comments, int min_score) {\
    ContentList* comments = calloc(1, sizeof(ContentList));
    if (!comments) {
        return NULL;
    }
    ContentItem* original_post = calloc(1, sizeof(ContentItem));
    if (!original_post) {
        return NULL;
    }

    if (parse_webpage_content(content, num_urls, website_type, comments, original_post, max_num_comments, min_score)) {
        return NULL;
    }

    char* response_msg = stringify_content_response(comments, original_post, max_length);
    free(comments);
    free(original_post);
    if (!response_msg) {
        return NULL;
    }
    return response_msg;
}