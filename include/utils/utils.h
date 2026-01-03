#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// Returns len of url-decoded string
int url_decoded_str_len(char* src);

// Decodes url-encoded string
void decode_url(char* dst, char* src, int src_len);

// String comparison function with the option for case sensitivity
bool str_equals(const char* a, const char* b, bool case_sensitive);

#endif