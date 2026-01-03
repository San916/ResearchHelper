#include "test_http_utils.h"
#include "http.h"
#include <stdio.h>

void display_http_request(HttpRequest* req) {
    printf("--------------------------------------\n");
    printf("%s %s %s\n", req->method, req->path, req->version);
    for (int i = 0; i < req->num_headers; i++) {
        HttpHeader cur_header = req->headers[i];
        printf("%s : %s\n", cur_header.key, cur_header.value);
    }
    printf("--------------------------------------\n");
}