#include "http.h"
#include "http_errors.h"
#include "handlers.h"
#include "utils.h"
#include "web_crawler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>

static int add_content_length(HttpResponse* resp) {
    char body_length_str[32];
    sprintf(body_length_str, "%d", resp->body_length);
    return set_header(resp->headers, &resp->num_headers, "Content-Length", body_length_str);
}

HttpResponse handle_home_html(HttpRequest* req) {
    HttpResponse resp = {0};
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

HttpResponse handle_home_js(HttpRequest* req) {
    HttpResponse resp = {0};
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
    char* response_msg = get_content_list(input, &status_code, MAX_RESPONSE_BODY_LEN);
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
    resp.body_length = (int)strlen(response_msg);
    
    if (add_content_length(&resp) != 0) {
        free(response_msg);
        free(resp.body);
        return handle_500();
    }
    
    free(response_msg);
    resp.close_connection = !req->keep_alive;
    return resp;
}

HttpResponse handle_content_request(HttpRequest* req) {
    HttpResponse resp = {0};
    
    if (strcmp(req->method, "GET") != 0) {
        return handle_405(req);
    }
    
    char* encoded_url = strstr(req->path, "?url=");
    if (!encoded_url) {
        return handle_400(req);
    }
    encoded_url = encoded_url + 5;

    int decoded_url_len = url_decoded_str_len(encoded_url);
    int encoded_url_len = strlen(encoded_url);
    char* decoded_url = malloc(decoded_url_len + 1);
    if (!decoded_url) return handle_500();
    decoded_url[decoded_url_len] = '\0';
    decode_url(decoded_url, encoded_url, encoded_url_len);

    printf("URL: %s\n", decoded_url);
    
    int status_code = 0;
    char* response_msg = get_content_item(decoded_url, &status_code, MAX_RESPONSE_BODY_LEN);
    if (!response_msg) {
        return handle_500();
    }

    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "application/json; charset=utf-8") != 0) {
        return handle_500();
    }

    printf("RESPONSE BODY: %s\n", response_msg);
    resp.body = malloc(strlen(response_msg) + 1);
    if (!resp.body) {
        return handle_500();   
    } 
    strcpy(resp.body, response_msg);
    resp.body_length = (int)strlen(response_msg);

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

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/html; charset=utf-8") != 0) {
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