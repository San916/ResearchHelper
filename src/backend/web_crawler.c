#include "web_crawler.h"
#include <curl/curl.h>

// Stub function
size_t write_memory_callback(char* contents, size_t size, size_t nmemb, void* userp) {
    printf("Adding string: %.*s\n", (int)nmemb, contents);
    printf("Size(%d) | nmemb(%d)", (int)size, (int)nmemb);
    // size_t size_to_add = size * nmemb;
    // size_t total_size = size_to_add + strlen(contents);

    return 0;
}

QueryResponse* input_query(char* input, int* status_code) {
    char* response = malloc(CURL_MAX_WRITE_SIZE);
    CURL* curl_handle;
    CURLcode response_code;

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
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)response);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);

    response_code = curl_easy_perform(curl_handle);

    if(response_code != CURLE_OK) {
        fprintf(stderr, "error: %s\n", curl_easy_strerror(response_code));
    }
    curl_easy_cleanup(curl_handle);
    free(response);
    return NULL;
}