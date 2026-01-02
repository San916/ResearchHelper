#include "http.h"
#include "http_errors.h"
#include "handlers.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

HttpResponse handle_home_html(HttpRequest* req) {
    HttpResponse resp = {0};
    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/html") != 0) {
        return handle_500();
    }

    const char* html = "<!DOCTYPE html><html><head><link rel=\"stylesheet\" href=\"main.css\"></head><body><h1>Hello world!</h1></body></html>";
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

HttpResponse handle_home_css(HttpRequest* req) {
    HttpResponse resp = {0};
    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/css") != 0) {
        return handle_500();
    }

    const char* css = "h1 {\nbackground-color: #ff0000\n}";
    int len = (int)strlen(css);

    resp.body = malloc(len + 1);
    if (!resp.body) {
        return handle_500();
    }

    strcpy(resp.body, css);
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

    const char* html = "<html><body><h1>About me</h1></body></html>";
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