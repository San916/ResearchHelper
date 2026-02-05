#include "test_utils.h"
#include <stdio.h>
#include <stdlib.h>

char* read_file(const char* file_name) {
    FILE *fp = fopen(file_name, "rb");
    if (fp == NULL) return NULL;

    fseek(fp, 0, SEEK_END);
    long file_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (file_len == -1) {
        fclose(fp);
        return NULL;
    }

    char* buffer = malloc(file_len + 1);
    if (buffer == NULL) {
        fclose(fp);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_len, fp);
    if (bytes_read != file_len) {
        free(buffer);
        fclose(fp);
        return NULL;
    }
    buffer[file_len] = '\0';

    fclose(fp);

    return buffer;
}