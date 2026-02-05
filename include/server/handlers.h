#ifndef HANDLERS_H
#define HANDLERS_H

typedef struct HttpRequest HttpRequest;
typedef struct HttpResponse HttpResponse;

HttpResponse handle_home_html(HttpRequest* req);
HttpResponse handle_home_css(HttpRequest* req);
HttpResponse handle_home_js(HttpRequest* req);
HttpResponse handle_submit(HttpRequest* req);
HttpResponse handle_content_request(HttpRequest* req);

#endif