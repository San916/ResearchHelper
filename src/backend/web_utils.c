#include "web_utils.h"

#include <stdio.h>

// REQUIRES: Path to .env starting at root/build/
// EFFECTS: Loads .env files to be accessed with getenv()
// Works for KEY=VALUE with no quotes
void load_env(const char *file_name) {
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
// EFFECTS: Callback given to curl_easy_perform. Adds nmemb bytes of content onto the data referenced in data_ptr
size_t write_memory_callback(char* content, size_t size, size_t nmemb, void* data_ptr) {
    struct WritebackData *chunk = (struct WritebackData *)data_ptr;
    size_t size_to_add = nmemb;
    size_t new_size = size_to_add + chunk->size;
    size_t old_strlen = chunk->size - 1;
    size_t new_strlen = new_size - 1;

    char* temp_ptr = realloc(chunk->data, new_size);
    if (!temp_ptr) return 0;
    chunk->data = temp_ptr;
    temp_ptr = NULL;

    chunk->size = new_size;
    chunk->data[new_strlen] = '\0';
    strncpy(&(chunk->data[old_strlen]), content, size_to_add);

    return strlen(chunk->data) - old_strlen;
}

// EFFECTS: Creates and returns curl handle after basic setup
CURL* create_curl_handle() {
    CURL* curl_handle;
    curl_handle = curl_easy_init();
    if (!curl_handle) return NULL;

    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "curl/8.16.0");
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, (long)1);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, (long)5);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);

    return curl_handle;
}

// EFFECTS: Creates and returns curl headers after basic setup
struct curl_slist* create_curl_headers() {
    struct curl_slist* headers = NULL;
    return headers;
}

// EFFECTS: Cleans up curl handle
void destroy_curl_handle(CURL* curl_handle) {
    curl_easy_cleanup(curl_handle);
}

// EFFECTS: Cleans up curl headers
void destroy_curl_headers(struct curl_slist* headers) {
    curl_slist_free_all(headers);
}

// REQUIRES: Url and curl handle
// EFEFCTS: Returns webpage content as string
char* fetch_webpage_content(const char* url, int* status_code, CURL* curl_handle, struct curl_slist* headers) {
    if (!curl_handle || !url) {
        return NULL;
    }
    CURLcode response_code;
    WritebackData writeback = {0};

    writeback.data = malloc(1);
    if (!writeback.data) return NULL;
    writeback.size = 1;
    writeback.data[0] = '\0';

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&writeback);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

    response_code = curl_easy_perform(curl_handle);

    if(response_code != CURLE_OK) {
        fprintf(stderr, "error: %s\n", curl_easy_strerror(response_code));
        free(writeback.data);
        return NULL;
    }
    
    return writeback.data;
}