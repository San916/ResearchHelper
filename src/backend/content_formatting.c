#include "content_formatting.h"
#include "webpage_parsing.h"
#include "json.h"
#include <stdlib.h>

// REQUIRES: Google search api response in JSON format with key "items"
// EFFECTS: Structures query response items and returns
QueryResponse* parse_google_query_response(const char* input) {
    QueryResponse* response = malloc(sizeof(QueryResponse));
    if (!response) return NULL;

    int num_elems = 0;
    char* items = get_json_value(input, "items");
    if (!items) {
        free(response);
        return NULL;
    }
    char** items_array = separate_array(items, &num_elems, MAX_NUM_RESPONSES);
    if (!items_array) {
        free(items);
        free(response);
        return NULL;
    }

    response->num_responses = 0;
    for (int i = 0; i < num_elems; i++) {
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
    for (int i = 0; i < num_elems; i++) {
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
    
    int current_position = 0;
    int remaining = max_length;
    current_position += snprintf(response_msg + current_position, remaining - current_position, "{\"results\":[");
    
    for (int i = 0 ; i < query_response->num_responses; i++) {
        if (i > 0) {
            current_position += snprintf(response_msg + current_position, remaining - current_position, ",");
        }
        current_position += snprintf(response_msg + current_position, remaining - current_position, 
            "{\"title\":%s,\"link\":%s}", 
            query_response->responses[i].title,
            query_response->responses[i].link);
        
        if (current_position >= remaining - 10) {
            break;
        }
    }

    current_position += snprintf(response_msg + current_position, remaining - current_position, "]}");
    if (current_position + 1 >= remaining) {
        free(response_msg);
        return NULL;
    }
    response_msg[current_position] = '\0';

    return response_msg;
}

// REQUIRES: Google search api response as a string
// EFFECTS: Creates a string containing relevant information from response, in JSON format
char* structure_google_query_response(const char* content, size_t max_length) {
    QueryResponse* query_response = parse_google_query_response(content);
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
// EFFECTS: Structures content and returns
ContentList* parse_webpage_content(const char* content, WebsiteType website_type) {
    ContentList* content_list = calloc(1, sizeof(ContentList));
    if (!content_list) {
        return NULL;
    }
    content_list->num_items = 0;

    switch (website_type) {
        case WEBSITE_REDDIT:
            break;
        case WEBSITE_STACKOVERFLOW:
            int status_code = parse_stackoverflow_content(content, content_list);
            if (status_code) {
                goto free_return_null;
            }
            break;
        case WEBSITE_GITHUB:
            break;
        case WEBSITE_STUB:
            // Return a stub
            content_list->num_items++;
            strcpy(content_list->items[0].content_body, "\"<code>int i = 0;</code>\"");
            content_list->items[0].score = 10;
            break;
        default:
            break;
    }

    return content_list;

free_return_null:
    free(content_list);
    return NULL;
}

// REQUIRES: Structured webpage content, max response length
// EFFECTS: Turns content struct into a JSON string
char* stringify_content_response(ContentList* content, size_t max_length) {
    char* response_msg = calloc(1, max_length);
    if (!response_msg) {
        return NULL;
    }

    int current_position = 0;
    int remaining = max_length;
    current_position += snprintf(response_msg + current_position, remaining - current_position, "{\"content\":[");

    for (int i = 0 ; i < content->num_items; i++) {
        if (i > 0) {
            current_position += snprintf(response_msg + current_position, remaining - current_position, ",");
        }
        current_position += snprintf(response_msg + current_position, remaining - current_position, 
            "{\"content_body\":%s,\"score\":%d}", 
            content->items[i].content_body,
            content->items[i].score
        );
        
        if (current_position >= remaining - 10) {
            break;
        }
    }

    current_position += snprintf(response_msg + current_position, remaining - current_position, 
        "],\"count\":%d}",
        content->num_items
    );
    if (current_position + 1 >= remaining) {
        free(response_msg);
    }

    response_msg[current_position] = '\0';

    return response_msg;
}

char* structure_webpage_content_response(const char* content, WebsiteType website_type, size_t max_length) {
    ContentList* content_list = parse_webpage_content(content, website_type);
    if (!content_list) {
        return NULL;
    }
    printf("PRINTING CONTENT_LIST:\n");
    for (int i = 0; i < content_list->num_items; i++) {
        printf("CONTENT_LIST->ITEMS[%d].CONTENT_BODY: %s\n", i, content_list->items[i].content_body);
    }

    char* response_msg = stringify_content_response(content_list, max_length);
    free(content_list);
    if (!response_msg) {
        return NULL;
    }
    return response_msg;
}