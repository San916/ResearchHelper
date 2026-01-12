#include "utils.h"
#include <stdlib.h>
#include <ctype.h>

// REQUIRES: string pointer
// EFFECTS: Returns pointer to end of whitespace
char* skip_whitespace(char* src) {
    char* end = src;
    while (*end && isspace(*end)) {
        end++;
    }
    return end;
}

// REQUIRES: url encoded src string
// EFFECTS: Returns the length of the decoded string
int url_decoded_str_len(char* src) {
    int len = 0;
    while(*src) {
        if (*src == '%' && *(src + 1) && *(src + 2)) {
            len++;
            src += 3;
        } else {
            len++;
            src++;
        }
    }
    return len;
}

// REQUIRES: Correctly allocated dst array, url encoded src string, length of src string
// MODIFIES: Decodes src string into dst
// EFFECTS: Decodes src string with url decode
void decode_url(char* dst, char* src, int src_len) {
    int dst_index = 0;
    for (int src_index = 0; src_index < src_len; src_index++) {
        if (src[src_index] == '%' && src[src_index + 1] && src[src_index + 2]) {
            char hex[3] = {
                src[src_index + 1], src[src_index + 2], '\0'
            };
            dst[dst_index++] = (char)strtol(hex, NULL, 16);
            src_index += 2;
        } else if (src[src_index] == '+') {
            dst[dst_index++] = ' ';
        } else {
            dst[dst_index++] = src[src_index];
        }
    }
    dst[dst_index] = '\0';
}

bool str_equals(const char* a, const char* b, bool case_sensitive) {
    if (!a || !b) return false;
    if (case_sensitive) return strcmp(a, b) == 0;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
        a++;
        b++;
    }
    return (*a == *b);
}