#ifndef HANDLERS_H
#define HANDLERS_H

#include "http.h"

HttpResponse handle_home_html(HttpRequest* req);
HttpResponse handle_home_css(HttpRequest* req);
HttpResponse handle_home_js(HttpRequest* req);
HttpResponse handle_submit(HttpRequest* req);
HttpResponse handle_about(HttpRequest* req);

#endif