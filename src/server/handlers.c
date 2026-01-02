#include "http.h"
#include "http_errors.h"
#include "handlers.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int add_content_length(HttpResponse* resp) {
    char body_length_str[32];
    sprintf(body_length_str, "%d", resp->body_length);
    return set_header(resp->headers, &resp->num_headers, "Content-Length", body_length_str);
}

HttpResponse handle_home_html(HttpRequest* req) {
    HttpResponse resp = {0};
    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/html") != 0) {
        return handle_500();
    }

    FILE *fp = fopen("./../frontend/index.html", "rb");
    if (!fp) return handle_500();

    fseek(fp, 0, SEEK_END);
    long file_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (file_len <= 0 || file_len > MAX_RESPONSE_BODY_LEN - 1) {
        fclose(fp);
        return handle_500();
    }

    resp.body = malloc((int)file_len + 1);
    if (!resp.body) {
        fclose(fp);
        return handle_500();
    }
    size_t bytes_read = fread(resp.body, 1, file_len, fp);
    fclose(fp);
    if ((long)bytes_read != file_len) {
        free(resp.body);
        return handle_500();
    }

    resp.body[bytes_read] = '\0';
    resp.body_length = bytes_read;
    if (add_content_length(&resp) != 0) { 
        free(resp.body);
        return handle_500();
    }
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

    FILE *fp = fopen("./../frontend/css/main.css", "rb");
    if (!fp) return handle_500();

    fseek(fp, 0, SEEK_END);
    long file_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (file_len <= 0 || file_len > MAX_RESPONSE_BODY_LEN - 1) {
        fclose(fp);
        return handle_500();
    }

    resp.body = malloc((int)file_len + 1);
    if (!resp.body) {
        fclose(fp);
        return handle_500();
    }
    size_t bytes_read = fread(resp.body, 1, file_len, fp);
    fclose(fp);
    if ((long)bytes_read != file_len) {
        free(resp.body);
        return handle_500();
    }

    resp.body[bytes_read] = '\0';
    resp.body_length = bytes_read;
    if (add_content_length(&resp) != 0) { 
        free(resp.body);
        return handle_500();
    }
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

    FILE *fp = fopen("./../frontend/about.html", "rb");
    if (!fp) return handle_500();

    fseek(fp, 0, SEEK_END);
    long file_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (file_len <= 0 || file_len > MAX_RESPONSE_BODY_LEN - 1) {
        fclose(fp);
        return handle_500();
    }

    resp.body = malloc((int)file_len + 1);
    if (!resp.body) {
        fclose(fp);
        return handle_500();
    }
    size_t bytes_read = fread(resp.body, 1, file_len, fp);
    fclose(fp);
    if ((long)bytes_read != file_len) {
        free(resp.body);
        return handle_500();
    }

    resp.body[bytes_read] = '\0';
    resp.body_length = bytes_read;
    if (add_content_length(&resp) != 0) { 
        free(resp.body);
        return handle_500();
    }
    resp.close_connection = !req->keep_alive;
    return resp;
}