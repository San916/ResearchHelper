#include "json.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// REQUIRES: pointer to start quotes of JSON string content
// EFFECTS: returns pointer to the end of the JSON string
static char* find_end_of_json_str(char* src) {
    char* end = src + 1;
    while (*end) {
        if (*end == '"' && *(end - 1) != '\\') {
            break;
        }
        end++;
    }
    if (!*end || *end != '"') {
        return NULL;
    }
    return end;
}

// REQUIRES: pointer to start of JSON primitive (exluding string)
// EFFECTS: finds the end of the JSON primitive
static char* find_end_of_json_primitive(char* src) {
    char* end = src;
    while (*end && *end != ',' && *end != '}' &&
           *end != ']' && !isspace(*end)) {
        end++;
    }
    return end - 1;
}

// REQUIRES: pointer to the start of a JSON object or array
// EFFECTS: returns pointer to the end of object/array
static char* find_end_of_json_object_or_array(char* src) {
    char opening = *src;
    char closing = (opening == '{') ? '}' : ']';
    char* end = src + 1;
    int depth = 1;
    while (*end && depth > 0) {
        if (*end == opening) {
            depth++;
        }
        else if (*end == closing) {
            depth--;
        }
        else if (*end == '"') {
            end = find_end_of_json_str(end);
            end++;
            if (!*end) {
                break;
            }
        }
        end++;
    }
    if (depth) {
        return NULL;
    }
    return end - 1;
}

// REQUIRES: pointer to the start of a JSON value
// EFFECTS: returns the length of the JSON value
static char* find_end_of_json_value(char* value_start) {
    char* value_end = NULL;
    if (*value_start == '"') {
        value_end = find_end_of_json_str(value_start);
    }
    else if (*value_start == '{' || *value_start == '[') {
        value_end = find_end_of_json_object_or_array(value_start);
    }
    else {
        value_end = find_end_of_json_primitive(value_start);
    }
    if (!value_end) {
        return NULL;
    }
    return value_end;
}

// REQUIRES: JSON string, key must exist in the highest level
// EFFECTS: Returns JSON object from key as string, return NULL on fail
char* get_json_value(const char* src, const char* key) {
    if (!src || !key || !*src || !*key) {
        return NULL;
    }
    fflush(stdout);
    char key_json[32];
    int pattern_len = snprintf(key_json, sizeof(key_json), "\"%s\":", key);
    if (pattern_len >= sizeof(key_json)) {
        return NULL;
    }

    char* value_start = strstr(src, key_json);
    if (!value_start) return NULL;

    value_start = value_start + pattern_len;
    value_start = skip_whitespace(value_start);

    char* value_end = find_end_of_json_value(value_start);
    if (!value_end) return NULL;
    size_t value_len = value_end - value_start + 1;
    char* result = malloc(value_len + 1);
    if (!result) return NULL;

    memcpy(result, value_start, value_len);
    result[value_len] = '\0';

    return result;
}

// REQUIRES: JSON array as string
// EFFECTS: Returns an array of JSON objects
char** separate_array(char* src) {
    int capacity = 10;
    char** result = malloc(capacity * sizeof(char*));
    if (!result) return NULL;
    int num_elems = 0;

    src++;
    while (*src && *src != ']') {
        src = skip_whitespace(src);
        char* value_start = src;

        char* value_end = find_end_of_json_value(value_start);
        if (!value_end) return NULL;
        size_t value_len = value_end - value_start + 1;
        char* element = malloc(value_len + 1);
        if (!element) {
            for (int i = 0; i < num_elems; i++) {
                free(result[i]);
            }
            free(result);
            return NULL;
        }

        memcpy(element, value_start, value_len);
        element[value_len] = '\0';
        result[num_elems] = element;
        num_elems++;


        src = value_end + 1; // Move past the last char of current JSON element

        src = skip_whitespace(src);
        if (!*src) {
            for (int i = 0; i < num_elems; i++) {
                free(result[i]);
            }
            free(result);
            return NULL;
        }
        if (*src == ',') {
            src++;
            src = skip_whitespace(src);
            if (!*src || *src == '}') {
                for (int i = 0; i < num_elems; i++) {
                    free(result[i]);
                }
                free(result);
                return NULL;
            }
        }

        if (num_elems >= capacity) {
            capacity++;
            char** new_result = realloc(result, capacity * sizeof(char*));
            if (!new_result) {
                for (int i = 0; i < num_elems; i++) {
                    free(result[i]);
                }
                free(result);
                return NULL;
            }
            result = new_result;
        }
    }
    return result;
}
