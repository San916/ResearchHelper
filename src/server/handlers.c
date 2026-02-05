#include "handlers.h"

#include "http.h"
#include "http_errors.h"

#include "web_crawler.h"

#include "utils.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>

static HttpRequestError add_content_length(HttpResponse* resp) {
    char body_length_str[32];
    sprintf(body_length_str, "%zu", resp->body_length);
    return set_header(resp->headers, &resp->num_headers, "Content-Length", body_length_str);
}

HttpResponse handle_home_html(HttpRequest* req) {
    HttpResponse resp = {0};

    if (strcmp(req->method, "GET") != 0 && strcmp(req->method, "HEAD") != 0) {
        return handle_405(req);
    }

    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/html; charset=utf-8") != 0) {
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

    resp.body_length = (size_t)file_len;
    if (add_content_length(&resp) != 0) { 
        fclose(fp);
        return handle_500();
    }
    resp.close_connection = !req->keep_alive;

    if (strcmp(req->method, "HEAD") == 0) {
        fclose(fp);
        return resp;
    }

    resp.body = malloc((size_t)file_len + 1);
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

    return resp;
}

HttpResponse handle_home_css(HttpRequest* req) {
    HttpResponse resp = {0};

    if (strcmp(req->method, "GET") != 0 && strcmp(req->method, "HEAD") != 0) {
        return handle_405(req);
    }

    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/css; charset=utf-8") != 0) {
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

    resp.body_length = (size_t)file_len;
    if (add_content_length(&resp) != 0) { 
        fclose(fp);
        return handle_500();
    }
    resp.close_connection = !req->keep_alive;

    if (strcmp(req->method, "HEAD") == 0) {
        fclose(fp);
        return resp;
    }

    resp.body = malloc((size_t)file_len + 1);
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

    return resp;
}

HttpResponse handle_home_js(HttpRequest* req) {
    HttpResponse resp = {0};

    if (strcmp(req->method, "GET") != 0 && strcmp(req->method, "HEAD") != 0) {
        return handle_405(req);
    }

    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/javascript; charset=utf-8") != 0) {
        return handle_500();
    }

    FILE *fp = fopen("./../frontend/js/main.js", "rb");
    if (!fp) return handle_500();

    fseek(fp, 0, SEEK_END);
    long file_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (file_len <= 0 || file_len > MAX_RESPONSE_BODY_LEN - 1) {
        fclose(fp);
        return handle_500();
    }

    resp.body_length = (size_t)file_len;
    if (add_content_length(&resp) != 0) { 
        fclose(fp);
        return handle_500();
    }
    resp.close_connection = !req->keep_alive;

    if (strcmp(req->method, "HEAD") == 0) {
        fclose(fp);
        return resp;
    }

    resp.body = malloc((size_t)file_len + 1);
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

    return resp;
}

HttpResponse handle_submit(HttpRequest* req) {
    HttpResponse resp = {0};
    
    if (strcmp(req->method, "POST") != 0) {
        return handle_405(req);
    }
    
    if (!req->body || strlen(req->body) == 0) {
        return handle_400(req);
    }
    
    char* input = strstr(req->body, "user_input=");
    if (!input) {
        return handle_400(req);
    }
    input = req->body + 11;

    int status_code = 0;
    GetContentListHandle* handle = get_content_list_handle(MAX_RESPONSE_BODY_LEN, req->max_num_responses);
    char* response_msg = get_content_list(input, &status_code, handle);
    free(handle);
    if (!response_msg) {
        return handle_500();
    }
    
    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "application/json; charset=utf-8") != 0) {
        free(response_msg);
        return handle_500();
    }

    resp.body = malloc(strlen(response_msg) + 1);
    if (!resp.body) {
        free(response_msg);
        return handle_500();   
    } 
    strcpy(resp.body, response_msg);
    resp.body_length = strlen(response_msg);
    free(response_msg);

    if (add_content_length(&resp) != 0) {
        free(resp.body);
        return handle_500();
    }
    
    resp.close_connection = !req->keep_alive;
    return resp;
}

HttpResponse handle_content_request(HttpRequest* req) {
    HttpResponse resp = {0};
    
    if (strcmp(req->method, "GET") != 0 && strcmp(req->method, "HEAD") != 0) {
        return handle_405(req);
    }
    
    char* encoded_url = strstr(req->path, "?url=");
    if (!encoded_url) {
        return handle_400(req);
    }
    encoded_url = encoded_url + 5;

    size_t decoded_url_len = url_decoded_str_len(encoded_url);
    size_t encoded_url_len = strlen(encoded_url);
    char* decoded_url = malloc(decoded_url_len + 1);
    if (!decoded_url) return handle_500();
    decoded_url[decoded_url_len] = '\0';
    decode_url(decoded_url, encoded_url, encoded_url_len);
    
    int status_code = 0;
    int escaped = 0;
    GetContentItemHandle* handle = get_content_item_handle(MAX_RESPONSE_BODY_LEN, req->max_num_comments, req->min_score);
    char* response_msg = get_content_item(decoded_url, &status_code, &escaped, handle);
    free(handle);
    if (!response_msg) {
        return handle_500();
    }

    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "application/json; charset=utf-8") != 0 ||
        set_header(resp.headers, &resp.num_headers, "Html-Escaped", escaped ? "true" : "false") != 0) {
        free(response_msg);
        return handle_500();
    }

    resp.body_length = strlen(response_msg);
    if (add_content_length(&resp) != 0) {
        return handle_500();
    }

    resp.close_connection = !req->keep_alive;

    if (strcmp(req->method, "HEAD") == 0) {
        free(response_msg);
        return resp;
    }

    resp.body = malloc(strlen(response_msg) + 1);
    if (!resp.body) {
        free(response_msg);
        return handle_500();
    } 
    strcpy(resp.body, response_msg);
    free(response_msg);

    return resp;
}