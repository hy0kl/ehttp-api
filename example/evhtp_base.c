#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <evhtp.h>

void
testcb(evhtp_request_t *req, void * a)
{
    const char * str = a;
    const char *v = NULL;

    v = evhtp_kv_find(req->uri->query, "v");
    if (v != NULL) {
        evbuffer_add(req->buffer_out, "v=", 2);
        evbuffer_add(req->buffer_out, v, strlen(v));
        evbuffer_add(req->buffer_out, "\n", sizeof("\n") - 1);
    }

    evbuffer_add(req->buffer_out, str, strlen(str));
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void
root(evhtp_request_t *req, void *arg)
{
    const char *json = "{\"code\":0,\"message\":\"ok\"}";
    evbuffer_add(req->buffer_out, json, strlen(json));
    evhtp_headers_add_header(req->headers_out,
        evhtp_header_new("Content-Type", "application/json; charset=UTF-8", 0, 0));
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void
default_router(evhtp_request_t *req, void *arg)
{
    const char *json = "{\"code\":0,\"message\":\"请求接口不存在\"}";
    evbuffer_add(req->buffer_out, json, strlen(json));
    evhtp_headers_add_header(req->headers_out,
        evhtp_header_new("Content-Type", "application/json; charset=UTF-8", 0, 0));
    evhtp_send_reply(req, EVHTP_RES_OK);
}

int
main(int argc, char ** argv) {
    uint64_t max_keepalives = 60;
    evbase_t * evbase = event_base_new();
    evhtp_t  * htp    = evhtp_new(evbase, NULL);

    //evhtp_set_parser_flags(htp, EVHTP_PARSE_QUERY_FLAG_LENIENT);
    evhtp_set_max_keepalive_requests(htp, max_keepalives);

    evhtp_set_cb(htp, "/simple/", testcb, "simple");
    evhtp_set_cb(htp, "/1/ping", testcb, "one");
    evhtp_set_cb(htp, "/1/ping.json", testcb, "two");
    evhtp_set_cb(htp, "/", root, NULL);
    evhtp_set_glob_cb(htp, "*", default_router, NULL);
#ifndef EVHTP_DISABLE_EVTHR
    evhtp_use_threads(htp, NULL, 8, NULL);
#endif
    evhtp_bind_socket(htp, "0.0.0.0", 8081, 2048);

    event_base_loop(evbase, 0);

    evhtp_unbind_socket(htp);
    evhtp_free(htp);
    event_base_free(evbase);

    return 0;
}

