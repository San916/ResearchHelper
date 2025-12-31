#include "http.h"
#include "http_errors.h"
#include "handlers.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

HttpResponse handle_home(HttpRequest* req) {
    HttpResponse resp = {0};
    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/html") != 0) {
        return handle_500();
    }

    const char* html = "<html><body><h1>Hello world!</h1></body></html>";
    int len = (int)strlen(html);

    resp.body = malloc(len + 1);
    if (!resp.body) {
        return handle_500();
    }

    strcpy(resp.body, html);
    resp.body_length = len;
    resp.close_connection = !req->keep_alive;
    
    return resp;
}

HttpResponse handle_about(HttpRequest* req) {
    HttpResponse resp = {0};
    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/html") != 0) {
        return handle_500();
    }

    const char* html = "<html><body><h1>About me!</h1></body></html>";
    int len = (int)strlen(html);

    resp.body = malloc(len + 1);
    if (!resp.body) {
        return handle_500();
    }

    strcpy(resp.body, html);
    resp.body_length = len;
    resp.close_connection = !req->keep_alive;
    
    return resp;
}