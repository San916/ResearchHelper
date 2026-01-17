#include "web_crawler.h"
#include "json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <curl/curl.h>

typedef enum {
    WEBSITE_UNKNOWN,
    WEBSITE_REDDIT,
    WEBSITE_STACKOVERFLOW,
    WEBSITE_GITHUB,
} WebsiteType;

// REQUIRES: Path to .env starting at root/build/
// EFFECTS: Loads .env files to be accessed with getenv()
static void load_env(const char *file_name) {
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) return;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");
        if (key && value) {
            _putenv_s(key, value);
        }
    }
    fclose(fp);
}

// REQUIRES: Non null-terminated content stream, size = 1 always, size of content stream nmemb, pointer to our WritebackData data_ptr
// MODIFIES: data pointed to by data_ptr. This data is assigned during curl_easy_setopt() with flag CURLOPT_WRITEDATA
// EFFECTS: Callback to curl_easy_perform. Adds nmemb bytes of content onto the data referenced in data_ptr
static size_t write_memory_callback(char* content, size_t size, size_t nmemb, void* data_ptr) {
    struct WritebackData *chunk = (struct WritebackData *)data_ptr;
    size_t size_to_add = nmemb;
    size_t new_size = size_to_add + chunk->size;
    size_t old_strlen = chunk->size - 1;
    size_t new_strlen = new_size - 1;

    char* temp_ptr = realloc(chunk->data, new_size);
    if (!temp_ptr) return 0;
    chunk->data = temp_ptr;
    temp_ptr = NULL;

    memcpy(&(chunk->data[old_strlen]), content, size_to_add);
    chunk->size = new_size;
    chunk->data[new_strlen] = '\0';
    return size_to_add;
}

// REQUIRES: Valud URL
// EFFECTS: Returns the type of url
static WebsiteType detect_website_type(const char* url) {
    if (strstr(url, "reddit.com")) return WEBSITE_REDDIT;
    if (strstr(url, "stackoverflow.com") || strstr(url, "stackexchange.com")) return WEBSITE_STACKOVERFLOW;
    if (strstr(url, "github.com")) return WEBSITE_GITHUB;
    return WEBSITE_UNKNOWN;
}

// REQUIRES: URL encoded input query
// EFFECTS: Returns input query in google search api format
char* get_google_search_url(char* input) {
    const char* api_key = getenv("GOOGLE_SEARCH_API_KEY");
    const char* search_engine_id = getenv("GOOGLE_SEARCH_ENGINE");
    if (!api_key || !search_engine_id) {
        load_env("..\\.env");
        api_key = getenv("GOOGLE_SEARCH_API_KEY");
        search_engine_id = getenv("GOOGLE_SEARCH_ENGINE");
    }
    char* url = calloc(1, MAX_CURL_URL_LEN);
    if (!url) return NULL;
    snprintf(url, MAX_CURL_URL_LEN, "https://www.googleapis.com/customsearch/v1?key=%s&cx=%s&q=%s", api_key, search_engine_id, input);
    printf("URL: %s\n", url);
    return url;
}

// REQUIRES: Query response in JSON format with key "items"
// EFFECTS: Structures query response items and returns
QueryResponse* structure_query_response(const char* input) {
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

// REQUIRES: Valid url
// EFEFCTS: Returns webpage content as string
char* fetch_webpage_content(const char* url, int* status_code) {
    if (strlen(url) >= MAX_CURL_URL_LEN) {
        return NULL;
    }
    CURL* curl_handle;
    CURLcode response_code;
    WritebackData writeback = {0};

    writeback.data = malloc(1);
    if (!writeback.data) return NULL;
    writeback.size = 1;
    writeback.data[0] = '\0';
    curl_handle = curl_easy_init();
    if (!curl_handle) {
        return NULL;
    }

    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, (long)1);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, (long)5);
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/8.16.0");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&writeback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);

    response_code = curl_easy_perform(curl_handle);

    printf("URL: %s\n", url);
    if(response_code != CURLE_OK) {
        fprintf(stderr, "error: %s\n", curl_easy_strerror(response_code));
        free(writeback.data);
        curl_easy_cleanup(curl_handle);
        return NULL;
    }
    
    curl_easy_cleanup(curl_handle);
    return writeback.data;
}

ContentList* parse_webpage_content(const char* html_content, const char* url, int* status_code) {
    ContentList* content_list = malloc(sizeof(ContentList));
    if (!content_list) {
        return NULL;
    }
    content_list->num_items = 0;
    
    WebsiteType site_type = detect_website_type(url);
    
    switch (site_type) {
        case WEBSITE_REDDIT:
            // parse_reddit_content(html_content, content_list);
            break;
        default:
            // parse_generic_content(html_content, content_list);
            break;
    }

    // Return a stub
    content_list->num_items++;
    strcpy(content_list->items[0].code, "\"```int i = 0;```\"");
    strcpy(content_list->items[0].discussion, "\"Blubbah\"");
    content_list->items[0].score = 10;
    return content_list;
}