#include "util.h"

const char *
get_message(g_error_code_e code)
{
    const char *msg = NULL;
    switch (code) {
        case CAN_NOT_OPEN_SERVER_JSON:
            msg = "Can not open server.json file.";
            break;

        case NEED_MORE_MEMORY:
            msg = "Out of memory.";
            break;

        case JSON_PARSE_FAILURE:
            msg = "json file paser error, please check it out.";
            break;

        case CAN_NOT_OPEN_ZLOG_CONF:
            msg = "Can not open zlog.conf";
            break;

        case CAN_NOT_GET_ZLOG_CATEGORY:
            msg = "Can not get zlog category, please check it out.";
            break;

        case LOST_MYSQL_CONFIG:
            msg = "Lost [mysql] config.";
            break;

        case LOST_MYSQL_MASTER_CONFIG:
            msg = "Lost [mysql.master] config.";
            break;

        case LOST_MYSQL_MASTER_HOST:
            msg = "Lost [mysql.master.host] config.";
            break;

        case LOST_MYSQL_MASTER_DBNAME:
            msg = "Lost [mysql.master.dbname] config.";
            break;

        case LOST_MYSQL_MASTER_USERNAME:
            msg = "Lost [mysql.master.username] config.";
            break;

        case LOST_MYSQL_MASTER_PASSWORD:
            msg = "Lost [mysql.master.password] config.";
            break;

        case LOST_MYSQL_SLAVES_CONFIG:
            msg = "Lost [mysql.slaves] config.";
            break;

        case LOST_MYSQL_SLAVES_HOST:
            msg = "Lost [mysql.slaves.host] config.";
            break;

        case LOST_MYSQL_SLAVES_DBNAME:
            msg = "Lost [mysql.slaves.dbname] config.";
            break;

        case LOST_MYSQL_SLAVES_USERNAME:
            msg = "Lost [mysql.slaves.username] config.";
            break;

        case LOST_MYSQL_SLAVES_PASSWORD:
            msg = "Lost [mysql.slaves.password] config.";
            break;

        case LOST_REDIS_CONFIG:
            msg = "Lost redis config node.";
            break;

        case LOST_REDIS_STORAGE:
            msg = "Lost redis storage node.";
            break;

        case LOST_REDIS_STORAGE_HOST:
            msg = "Lost redis storage host.";
            break;

        case LOST_REDIS_CACHE:
            msg = "Lost redis cache node.";
            break;

        case LOST_REDIS_CACHE_HOST:
            msg = "Lost redis cache host.";
            break;

        case LOST_CURL_CONFIG:
            msg = "Lost curl config node.";
            break;

        default:
            msg = "unreachable";
    }

    return msg;
}

void
log_uri(evhtp_request_t *req)
{
    zlog_info(g_zc, "uri: %s%s%s",
            req->uri->path->full,
            req->uri->query_raw ? "?" : "",
            req->uri->query_raw ? (char *)req->uri->query_raw : "");
}

void
default_router(evhtp_request_t *req, void *arg)
{
    log_uri(req);
    const char *json = "{\"code\":0,\"message\":\"请求接口不存在\"}";
    evbuffer_add(req->buffer_out, json, strlen(json));
    evhtp_headers_add_header(req->headers_out,
        evhtp_header_new("Content-Type", "application/json; charset=UTF-8", 0, 0));
    evhtp_send_reply(req, EVHTP_RES_OK);
}

void
clean(void)
{
    // 销毁数据库连接池
    ConnectionPool_free(&g_conf.mysql_master.pool);
    URL_free(&g_conf.mysql_master.url);

    zlog_fini();
}
