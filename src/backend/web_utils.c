#include "web_utils.h"
#include "webpage_parsing.h"
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
    temp_ptr = NULL;\

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
    if (!curl_handle) return;
    curl_easy_cleanup(curl_handle);
}

// EFFECTS: Cleans up curl headers
void destroy_curl_headers(struct curl_slist* headers) {
    if (!headers) return;
    curl_slist_free_all(headers);
}

// REQUIRES: Url and curl handle
// EFEFCTS: Returns webpage content as string
char* fetch_webpage_content(const char* url, int* status_code, CURL* curl_handle, struct curl_slist* headers) {
    if (strlen(url) >= MAX_CURL_URL_LEN || !curl_handle) {
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

// REQUIRES: URL encoded input query
// EFFECTS: Returns input query in google search api format
char* get_google_search_url(const char* input) {
    char* api_key = getenv("GOOGLE_SEARCH_API_KEY");
    char* search_engine_id = getenv("GOOGLE_SEARCH_ENGINE");
    if (!api_key || !search_engine_id) {
        load_env("..\\.env");
        api_key = getenv("GOOGLE_SEARCH_API_KEY");
        search_engine_id = getenv("GOOGLE_SEARCH_ENGINE");
    }
    char* url = calloc(1, MAX_CURL_URL_LEN);
    if (!url) return NULL;
    snprintf(url, MAX_CURL_URL_LEN, "https://www.googleapis.com/customsearch/v1?key=%s&cx=%s&q=%s", api_key, search_engine_id, input);
    return url;
}

// REQUIRES: Valud url
// EFFECTS: Returns the specific type of website in the url
WebsiteType detect_website_type(const char* url) {
    if (strstr(url, "reddit.com")) return WEBSITE_REDDIT;
    if (strstr(url, "stackoverflow.com") || strstr(url, "stackexchange.com")) return WEBSITE_STACKOVERFLOW;
    if (strstr(url, "github.com")) return WEBSITE_GITHUB;
    return WEBSITE_UNKNOWN;
}

// REQUIRES: Valid stackoverflow post url
// EFFECTS: Returns the question id as string
char* extract_stackoverflow_question_id(const char* url) {
    const char* pattern = "questions/";
    char* first_slash = strstr(url, pattern);
    if (!first_slash) {
        return NULL;
    }
    first_slash = first_slash + strlen(pattern);
    char* second_slash = strchr(first_slash, '/');
    size_t question_id_length = second_slash - first_slash;
    if (question_id_length > 12) {
        return NULL;
    }
    char* question_id = calloc(1, question_id_length + 1);
    strncpy(question_id, first_slash, question_id_length);
    return question_id;
}

// REQUIRES: Valid stackoverflow post url
// EFFECTS: Returns the question id as string
char* extract_reddit_question_id(const char* url) {
    const char* pattern = "comments/";
    char* first_slash = strstr(url, pattern);
    if (!first_slash) {
        return NULL;
    }
    first_slash = first_slash + strlen(pattern);
    char* second_slash = strchr(first_slash, '/');
    size_t question_id_length = second_slash - first_slash;
    if (question_id_length > 12) {
        return NULL;
    }
    char* question_id = calloc(1, question_id_length + 1);
    strncpy(question_id, first_slash, question_id_length);
    return question_id;
}

// REQUIRES: Valid url and WebsiteType
// EFFECTS: Executes different curl_handle setups according to the website type
// new url to curl (some websites may want us to append .json, add .api, etc)
char* web_specific_setup(const char* url, WebsiteType type, CURL* curl_handle, struct curl_slist** headers, int* escaped) {
    char* new_url = calloc(1, MAX_CURL_URL_LEN);
    if (!new_url) {
        return NULL;
    }

    switch (type) {
        case WEBSITE_REDDIT: {
            char* question_id = extract_reddit_question_id(url);
            if (!question_id) {
                return NULL;
            }

            snprintf(new_url, MAX_CURL_URL_LEN, "https://www.reddit.com/comments/%s.json?limit=%d&depth=%d&sort=top", question_id, REDDIT_API_LIMIT, REDDIT_API_DEPTH);
            free(question_id);

            char* reddit_id = getenv("REDDIT_ID");
            if (!reddit_id) {
                load_env("..\\.env");
                reddit_id = getenv("REDDIT_ID");
            }

            char user_agent[MAX_USER_AGENT_LEN] = {0};
            snprintf(user_agent, MAX_USER_AGENT_LEN, "windows:ResearchHelper:v1.0 (by %s)", reddit_id);
            curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, user_agent);
            curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");
            curl_easy_setopt(curl_handle, CURLOPT_URL, new_url);

            *escaped = 1;
            break;
        } case WEBSITE_STACKOVERFLOW: {
            char* question_id = extract_stackoverflow_question_id(url);
            if (!question_id) {
                return NULL;
            }

            snprintf(new_url, MAX_CURL_URL_LEN, "https://api.stackexchange.com/2.3/questions/%s/answers?site=stackoverflow&filter=withbody", question_id);
            free(question_id);

            *headers = curl_slist_append(*headers, "Accept: application/json");
            curl_easy_setopt(curl_handle, CURLOPT_URL, new_url);
            curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");
            curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "ResearchHelper/1.0");

            break;
        } case WEBSITE_GITHUB: {
            break;
        } default: {
            break;
        }
    }

    
    return new_url;
}