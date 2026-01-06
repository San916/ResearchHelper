#include "web_crawler.h"
#include <curl/curl.h>

// Stub function
size_t write_memory_callback(char* content, size_t size, size_t nmemb, void* userdata) {
    struct WritebackData *chunk = (struct WritebackData *)userdata;
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
    fflush(stdout);
    return size_to_add;
}

QueryResponse* input_query(char* input, int* status_code) {
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

    char* encoded_input = curl_easy_escape(curl_handle, input, strlen(input));
    if (!encoded_input) return NULL;
    char url[512];
    snprintf(url, sizeof(url), "https://api.duckduckgo.com/?q=%s&format=json&no_html=1", encoded_input);
    curl_free(encoded_input);

    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, (long)1);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, (long)5);
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "web_crawler/1.0");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&writeback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);

    response_code = curl_easy_perform(curl_handle);

    if(response_code != CURLE_OK) {
        fprintf(stderr, "error: %s\n", curl_easy_strerror(response_code));
        free(writeback.data);
        curl_easy_cleanup(curl_handle);
        return NULL;
    }
    free(writeback.data);
    curl_easy_cleanup(curl_handle);
    return NULL;
}