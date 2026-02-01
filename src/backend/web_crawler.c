#include "web_crawler.h"

#include "web_utils.h"
#include "web_crawler_config.h"
#include "web_crawler_priv.h"
#include "content_formatting.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <curl/curl.h>

GetContentListHandle* get_content_list_handle(size_t max_content_length, size_t max_num_responses) {
    GetContentListHandle* handle = malloc(sizeof(GetContentListHandle));
    handle->max_content_length = max_content_length;
    handle->max_num_responses = max_num_responses;
    handle->env_path = "..\\.env";
    handle->create_curl_handle = create_curl_handle;
    handle->load_env = load_env;
    handle->get_search_url = get_google_search_url;
    handle->fetch_webpage_content = fetch_webpage_content;
    handle->structure_google_query_response = structure_google_query_response;
    handle->destroy_curl_handle = destroy_curl_handle;
    return handle;
}

GetContentItemHandle* get_content_item_handle(size_t max_content_length, size_t max_num_comments, int min_score) {
    GetContentItemHandle* handle = malloc(sizeof(GetContentItemHandle));
    handle->max_content_length = max_content_length;
    handle->max_num_comments = max_num_comments;
    handle->min_score = min_score;
    handle->env_path = "..\\.env";
    handle->create_curl_handle = create_curl_handle;
    handle->create_curl_headers = create_curl_headers;
    handle->detect_website_type = detect_website_type;
    handle->load_env = load_env;
    handle->web_specific_setup = web_specific_setup;
    handle->fetch_webpage_content = fetch_webpage_content;
    handle->structure_webpage_content_response = structure_webpage_content_response;
    handle->destroy_curl_handle = destroy_curl_handle;
    handle->destroy_curl_headers = destroy_curl_headers;
    return handle;
}

// REQUIRES: url encoded google search query, maximum length, status code address
// EFFECTS: execute google search api call, structure api response, return as JSON formatted string
char* get_content_list(const char* query, int* status_code, GetContentListHandle* handle) {
    char* content_list = NULL;
    CURL* curl_handle = handle->create_curl_handle();
    if (!curl_handle) {
        goto destroy_curl_return;
    }

    handle->load_env(handle->env_path);
    char* search_url = handle->get_search_url(query);
    if (!search_url) {
        goto destroy_curl_return;
    }

    char* webpage_content = handle->fetch_webpage_content(search_url, status_code, curl_handle, NULL);
    free(search_url);
    if (!webpage_content) {
        goto destroy_curl_return;
    }
    content_list = handle->structure_google_query_response(webpage_content, handle->max_content_length, handle->max_num_responses);
    free(webpage_content);

destroy_curl_return:
    handle->destroy_curl_handle(curl_handle);
    return content_list;
}

// REQUIRES: url, status code address
// EFFECTS: Looks through webpage in the url, returns relevant information as JSON formatted string
char* get_content_item(const char* url, int* status_code, int* escaped, GetContentItemHandle* handle) {
    char* content_json = NULL;
    CURL* curl_handle = handle->create_curl_handle();
    struct curl_slist* headers = handle->create_curl_headers();
    if (!curl_handle) {
        goto destroy_curl_return;
    }

    WebsiteType website_type = handle->detect_website_type(url);

    handle->load_env(handle->env_path);
    size_t num_urls = 0;
    char** new_urls = handle->web_specific_setup(url, website_type, curl_handle, &headers, escaped, handle->max_num_comments, &num_urls);
    if (!new_urls) {
        goto destroy_curl_return;
    }

    char** webpage_content = malloc(num_urls * sizeof(char*));
    if (!webpage_content) {
        goto destroy_curl_return;
    }
    for (size_t i = 0; i < num_urls; i++) {
        webpage_content[i] = handle->fetch_webpage_content(new_urls[i], status_code, curl_handle, headers);
        free(new_urls[i]);
        if (!webpage_content[i]) {
            for (size_t j = 0; j < i; j++) {
                free(webpage_content[j]);
            }
            goto destroy_curl_return;
        }
    }
    free(new_urls);
    content_json = handle->structure_webpage_content_response(webpage_content, num_urls, website_type, handle->max_content_length, handle->max_num_comments, handle->min_score);
    free (webpage_content);

destroy_curl_return:
    handle->destroy_curl_handle(curl_handle);
    handle->destroy_curl_headers(headers);
    return content_json;
}