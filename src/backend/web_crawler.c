#include "web_crawler.h"

#include "web_utils.h"
#include "web_crawler_config.h"
#include "content_formatting.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <curl/curl.h>

// REQUIRES: url encoded google search query, maximum length, status code address
// EFFECTS: execute google search api call, structure api response, return as JSON formatted string
char* get_content_list(const char* query, int* status_code, size_t max_content_length, size_t max_num_responses) {
    char* content_list = NULL;
    CURL* curl_handle = create_curl_handle();
    if (!curl_handle) {
        goto destroy_curl_return;
    }

    load_env("..\\.env");
    char* search_url = get_google_search_url(query);
    if (!search_url) {
        goto destroy_curl_return;
    }

    char* webpage_content = fetch_webpage_content(search_url, status_code, curl_handle, NULL);
    free(search_url);
    if (!webpage_content) {
        goto destroy_curl_return;
    }

    content_list = structure_google_query_response(webpage_content, max_content_length, max_num_responses);
    free(webpage_content);

destroy_curl_return:
    destroy_curl_handle(curl_handle);
    return content_list;
}

// REQUIRES: url, status code address
// EFFECTS: Looks through webpage in the url, returns relevant information as JSON formatted string
char* get_content_item(const char* url, int* status_code, int* escaped, size_t max_content_length, size_t max_num_comments, int min_score) {
    char* content_json = NULL;
    CURL* curl_handle = create_curl_handle();
    if (!curl_handle) {
        goto destroy_curl_return;
    }
    struct curl_slist* headers = create_curl_headers();

    WebsiteType website_type = detect_website_type(url);

    load_env("..\\.env");
    size_t num_urls = 0;
    char** new_urls = web_specific_setup(url, website_type, curl_handle, &headers, escaped, max_num_comments, &num_urls);
    if (!new_urls) {
        goto destroy_curl_return;
    }

    char** webpage_content = malloc(num_urls * sizeof(char*));
    if (!webpage_content) {
        goto destroy_curl_return;
    }
    for (size_t i = 0; i < num_urls; i++) {
        webpage_content[i] = fetch_webpage_content(new_urls[i], status_code, curl_handle, headers);
        free(new_urls[i]);
        if (!webpage_content[i]) {
            for (size_t j = 0; j < i; j++) {
                free(webpage_content[j]);
            }
            goto destroy_curl_return;
        }
    }
    free(new_urls);
    content_json = structure_webpage_content_response(webpage_content, num_urls, website_type, max_content_length, max_num_comments, min_score);
    free (webpage_content);

destroy_curl_return:
    destroy_curl_handle(curl_handle);
    return content_json;
}