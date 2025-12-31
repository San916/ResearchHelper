#include "utils.h"
#include <ctype.h>

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