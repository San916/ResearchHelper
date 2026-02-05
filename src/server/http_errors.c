#include "http_errors.h"
#include "http.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char* handle_build_http_response_error(HttpResponse* resp) {
    char* response = calloc(1, 1 + strlen(MINIMAL_500_RESPONSE));
    strcpy(response, MINIMAL_500_RESPONSE);
    return response;
}

// Handles the different status codes during parse_http_request()
// TODO: Add different responses depending on status code
HttpResponse handle_parse_http_request_error(HttpRequest* req, int status_code) {
    // For now, just return 400 Bad Request
    return handle_400(req);
}

// Bad request
HttpResponse handle_400(HttpRequest* req) {
    HttpResponse resp = {0};

    resp.status_code = 400;
    resp.status_text = "Bad Request";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/plain") != 0) {
        return handle_500();
    };

    const char* msg = "400 - Bad request";
    size_t len = strlen(msg);

    resp.body = malloc(len + 1);
    if (!resp.body) {
        return handle_500();
    }

    strcpy(resp.body, msg);
    resp.body_length = len;
    resp.close_connection = false;

    return resp;
}

// Default handler when no route found
HttpResponse handle_404(HttpRequest* req) {
    HttpResponse resp = {0};

    resp.status_code = 404;
    resp.status_text = "Not Found";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/plain") != 0) {
        return handle_500();
    };

    const char* msg = "404 - Page not found";
    size_t len = strlen(msg);

    resp.body = malloc(len + 1);
    if (!resp.body) return handle_500();

    strcpy(resp.body, msg);
    resp.body_length = len;
    resp.close_connection = false;

    return resp;
}

// Bad method
HttpResponse handle_405(HttpRequest* req) {
    HttpResponse resp = {0};

    resp.status_code = 405;
    resp.status_text = "Bad method";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/plain") != 0) {
        return handle_500();
    };

    const char* msg = "405 - Bad method";
    size_t len = strlen(msg);

    resp.body = malloc(len + 1);
    if (!resp.body) return handle_500();

    strcpy(resp.body, msg);
    resp.body_length = len;
    resp.close_connection = false;

    return resp;
}

// Assuming somewhere the server failed, create a 500 response
HttpResponse handle_500() {
    HttpResponse resp = {0};

    resp.status_code = 500;
    resp.status_text = "Internal Server Error";

    resp.close_connection = true;

    return resp;
}