#include "http.h"
#include "http_errors.h"
#include "handlers.h"
#include "utils.h"
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

HttpResponse handle_home_js(HttpRequest* req) {
    HttpResponse resp = {0};
    resp.status_code = 200;
    resp.status_text = "OK";

    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/javascript") != 0) {
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

    int decoded_input_len = url_decoded_str_len(input);
    char *decoded_input = malloc(url_decoded_str_len(input) + 1);
    if (!decoded_input) return handle_500();
    decoded_input[decoded_input_len] = '\0';
    decode_url(decoded_input, input, decoded_input_len);
    input = decoded_input;
    
    resp.status_code = 200;
    resp.status_text = "OK";
    
    if (set_header(resp.headers, &resp.num_headers, "Content-Type", "text/plain") != 0) {
        free(decoded_input);
        return handle_500();
    }
    
    char response_msg[256];
    snprintf(response_msg, sizeof(response_msg), "You entered: %s", input);

    resp.body = malloc(strlen(response_msg) + 1);
    if (!resp.body) {
        free(decoded_input);
        return handle_500();   
    } 
    strcpy(resp.body, response_msg);
    resp.body_length = (int)strlen(response_msg);
    
    if (add_content_length(&resp) != 0) {
        free(resp.body);
        free(decoded_input);
        return handle_500();
    }
    
    free(decoded_input);
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