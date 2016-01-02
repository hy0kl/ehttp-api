#include "util.h"

static void
handler(int signo)
{
    fprintf(stderr, "Get one signal: %d. man sigaction.\n", signo);
    return;
}


void
signal_setup(void)
{
    static int signo[] = {
        SIGHUP,
        SIGINT,     /* ctrl + c */
        SIGCHLD,    /* 僵死进程或线程的信号 */
        SIGPIPE,
        SIGALRM,
        SIGUSR1,
        SIGUSR2,
        SIGTERM,
        //SIGCLD,

#ifdef  SIGTSTP
        /* background tty read */
        SIGTSTP,
#endif
#ifdef  SIGTTIN
        /* background tty read */
        SIGTTIN,
#endif
#ifdef SIGTTOU
        SIGTTOU,
#endif
        SIGNO_END
    };

    int i = 0;
    struct sigaction sa;
    //sa.sa_handler = SIG_IGN;    //设定接受到指定信号后的动作为忽略
    sa.sa_handler = handler;
    sa.sa_flags   = SA_SIGINFO;

    if (-1 == sigemptyset(&sa.sa_mask))   //初始化信号集为空
    {
        fprintf(stderr, "failed to init sa_mask.\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; SIGNO_END != signo[i]; i++)
    {
        //屏蔽信号
        if (-1 == sigaction(signo[i], &sa, NULL))
        {
            fprintf(stderr, "failed to ignore: %d\n", signo[i]);
            exit(EXIT_FAILURE);
        }
    }

    return;
}

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

        case API_OK:
            msg = "OK";
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

        case API_DOES_NOT_EXIST:
            msg = "Request interface does not exist.";
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
set_json_header(evhtp_request_t *req)
{
    evhtp_headers_add_header(req->headers_out,
        evhtp_header_new("Content-Type", "application/json; charset=UTF-8", 0, 0));
    evhtp_headers_add_header(req->headers_out,
        evhtp_header_new("Runtime-Enviroment", g_conf.env, 0, 0));
    evhtp_headers_add_header(req->headers_out,
        evhtp_header_new("Server", SERVER_NAME, 0, 0));
    evhtp_headers_add_header(req->headers_out,
        evhtp_header_new("Connection", "keep-alive", 0, 0));
}

g_error_code_e
get_master_db_link(Connection_T *db_link)
{
    *db_link = ConnectionPool_getConnection(g_conf.mysql_master.pool);
    return API_OK;
}

g_error_code_e
get_slave_db_link(Connection_T *db_link)
{
    int index    = rand() % g_conf.mysql_slaves_count;
    *db_link = ConnectionPool_getConnection(g_conf.mysql_slaves_array[index].pool);
    zlog_debug(g_zc, "select slave: %d", index);
    return API_OK;
}

void
build_base_json(cJSON *root_json, g_error_code_e code)
{
    assert(NULL != root_json);
    assert(cJSON_Object == root_json->type);

    cJSON_AddNumberToObject(root_json, RES_CODE, code);
    cJSON_AddStringToObject(root_json, RES_MSG, get_message(code));

    return;
}

void
default_router(evhtp_request_t *req, void *arg)
{
    log_uri(req);

    set_json_header(req);

    cJSON *root_json = cJSON_CreateObject();
    build_base_json(root_json, API_DOES_NOT_EXIST);
    cJSON *data = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "data", data);

    char *json = cJSON_PrintUnformatted(root_json);
    evbuffer_add(req->buffer_out, json, strlen(json));

    // 清除内存
    if (json) { free(json); }
    if (root_json) { cJSON_Delete(root_json); }

    evhtp_send_reply(req, EVHTP_RES_OK);
}

void
clean(void)
{
    // 销毁数据库主库连接池
    ConnectionPool_free(&g_conf.mysql_master.pool);
    URL_free(&g_conf.mysql_master.url);

    // 销毁数据库从库连接池
    int i;
    for (i = 0; i < g_conf.mysql_slaves_count; i++) {
        ConnectionPool_free(&g_conf.mysql_slaves_array[i].pool);
        URL_free(&g_conf.mysql_slaves_array[i].url);
    }

    zlog_fini();
}

