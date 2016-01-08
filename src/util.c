#include "util.h"

static void
handler(int signo)
{
    //fprintf(stderr, "Get one signal: %d. man sigaction.\n", signo);
    zlog_info(g_zc, "Get one signal: %d. man sigaction.", signo);
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
get_message(const g_error_code_e code)
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

        case REQUEST_METHOD_DOES_NOT_MATCH:
            msg = "Request method does not match.";
            break;

        case DATA_STRUCTURE_TYPE_DOES_NOT_MATCH:
            msg = "Data structure type does not match.";
            break;

        case MISSING_REQUIRED_PARAMETERS:
            msg = "Missing required parameters.";
            break;

        case SERVICE_UNAVAILABLE:
            msg = "Back-end service is unavailable.";
            break;

        case CURL_GET_WRONG:
            msg = "libcurl get wrong.";
            break;

        default:
            msg = "unreachable";
    }

    return msg;
}

void
log_uri(const evhtp_request_t *req)
{
    htp_method method = htparser_get_method(req->conn->parser);

    zlog_info(g_zc, "%s \"%s %s%s%s\"",
            "", // ip 占位
            htparser_get_methodstr_m(method), // HTTP_METHOD
            req->uri->path->full,
            req->uri->query_raw && htp_method_GET == method ? "?" : "",
            req->uri->query_raw && htp_method_GET == method ? (char *)req->uri->query_raw : "");
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

void keepalived_ping(evhtp_request_t *req, void *arg)
{
    log_uri(req);
    set_json_header(req);

    cJSON *root_json = cJSON_CreateObject();
    build_base_json(root_json, API_OK);
    cJSON *data = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, "data", data);
    cJSON_AddStringToObject(data, "ping", "pong");

    char *json = cJSON_PrintUnformatted(root_json);
    evbuffer_add(req->buffer_out, json, strlen(json));

    // 清除内存
    if (json) { free(json); }
    if (root_json) { cJSON_Delete(root_json); }

    evhtp_send_reply(req, EVHTP_RES_OK);
}

g_error_code_e
get_post_data_raw(const evhtp_request_t *req, char *buf, size_t buf_len)
{
    assert(NULL != req);
    assert(NULL != buf);

    if (htp_method_POST != htparser_get_method(req->conn->parser)) { return REQUEST_METHOD_DOES_NOT_MATCH; };

    size_t content_len = evhtp_request_content_len(req);
    size_t real_size   = content_len >= buf_len ? buf_len  - 1 : content_len;
    evbuffer_copyout(req->buffer_in, buf, real_size);
    buf[real_size] = '\0';

    return API_OK;
}

void
clean(void)
{
    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();

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

g_error_code_e
filter_request_parameters(
        const evhtp_request_t *req,
        const req_param_filter_t *filter,
        cJSON *json_obj)
{
    assert(NULL != req);
    assert(NULL != filter);
    assert(NULL != json_obj);
    assert(cJSON_Object == json_obj->type);

    g_error_code_e ret_code = API_OK;

    const char *value;

    for (; NULL != filter->param; filter++) {
        value = evhtp_kv_find(req->uri->query, filter->param);

        if (filter->required && NULL == value) {
            zlog_error(g_zc, "%s [required_parameter: %s]", get_message(MISSING_REQUIRED_PARAMETERS), filter->param);
            ret_code = MISSING_REQUIRED_PARAMETERS;
            break;
        }

        if (NULL == value) {
            if (REQ_PARAM_STRING == filter->type) {
                cJSON_AddStringToObject(json_obj, filter->param, "");
            } else if (REQ_PARAM_INT == filter->type) {
                cJSON_AddNumberToObject(json_obj, filter->param, 0);
            }
        } else {
            if (REQ_PARAM_STRING == filter->type) {
                cJSON_AddStringToObject(json_obj, filter->param, value);
            } else if (REQ_PARAM_INT == filter->type) {
                cJSON_AddNumberToObject(json_obj, filter->param, atoi(value));
            }
        }
    }

#if (_DEBUG_)
    char *json = cJSON_PrintUnformatted(json_obj);
    zlog_debug(g_zc, "[%s] JSON: %s", __func__, json);
    if (json) { free(json); }
#endif

    return ret_code;
}

curl_buf_t *
create_curl_buf(const size_t buf_len)
{
    assert(buf_len > 0);

    curl_buf_t *curl_buf = (curl_buf_t *)malloc(sizeof(curl_buf_t));
    if (NULL == curl_buf) {
        zlog_error(g_zc, "curl_buf %s", get_message(NEED_MORE_MEMORY));
        return NULL;
    }

    curl_buf->buf = (char *)malloc(buf_len);
    if (NULL == curl_buf->buf) {
        zlog_error(g_zc, "curl_buf->buf %s", get_message(NEED_MORE_MEMORY));
        free(curl_buf);
        return NULL;
    }

    curl_buf->size    = 0;
    curl_buf->buf_len = buf_len;
    curl_buf->buf[0]  = '\0';
    curl_buf->response_code = 0;

    zlog_debug(g_zc, "create_curl_buf successful.");

    return curl_buf;
}

void
delete_curl_buf(curl_buf_t *curl_buf)
{
    if (NULL != curl_buf) {
        if (NULL != curl_buf->buf) {
            free(curl_buf->buf);
        }
        free(curl_buf);
    }
}

static size_t
curl_write_buf_callback(
        void *contents,
        size_t size, size_t nmemb,
        void *userp)
{
    size_t realsize = size * nmemb;
    curl_buf_t *curl_buf = (curl_buf_t *)userp;

    /** 如果缓冲区不够容纳结果,扩充缓冲区 */
    if (realsize + 1 + curl_buf->size > curl_buf->buf_len) {
        curl_buf->buf = realloc(curl_buf->buf, curl_buf->size + realsize + 1);
        if (NULL == curl_buf->buf) {
            zlog_error(g_zc, "not enough memory (realloc returned NULL)");
            return 0;
        }

        curl_buf->buf_len = curl_buf->size + realsize + 1;
    }

    memcpy(&(curl_buf->buf[curl_buf->size]), contents, realsize);
    curl_buf->size += realsize;
    curl_buf->buf[curl_buf->size] = '\0';

    return realsize;
}

g_error_code_e
curl_get_api(const char *api, curl_buf_t *curl_buf)
{
    assert(NULL != api);
    assert(NULL != curl_buf);
    assert(NULL != curl_buf->buf);

    g_error_code_e code = API_OK;
    CURL *curl_handle;

    /* init the curl session */
    curl_handle = curl_easy_init();
    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, api);
    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_write_buf_callback);
    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)curl_buf);
    /* some servers don't like requests that are made without a user-agent field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT,
            "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.106 Safari/537.36");
    /* example.com is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    /* complete connection with milliseconds */
    curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT_MS, g_conf.curl_conf.connect_timeout_ms);
    /* complete with milliseconds */
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, g_conf.curl_conf.timeout_ms);
    /* 防止超时信号挂起 */
    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);

    /* get it! */
    curl_buf->result_code = curl_easy_perform(curl_handle);
    if (CURLE_OK != curl_buf->result_code) {
        code = CURL_GET_WRONG;
        zlog_error(g_zc, "[%s] curl_easy_perform() failed: %d %s", api,
                curl_buf->result_code, curl_easy_strerror(curl_buf->result_code));
        goto FINISH;
    }

    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &(curl_buf->response_code));

FINISH:
    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);
    zlog_debug(g_zc, "[api: %s] [response: %s]", api, curl_buf->buf);

    return code;
}

redisContext *
create_redis_storage_context(void)
{
    struct timeval timeout = {0, g_conf.redis_storage.timeout * 1000};

    redisContext *c = redisConnectWithTimeout(g_conf.redis_storage.host, g_conf.redis_storage.port, timeout);
    if (NULL == c || c->err) {
        if (c) {
            redisFree(c);
            c = NULL;
        }
        zlog_warn(g_zc, "Can NOT connect redis storage master: [host: %s] [port: %u]", g_conf.redis_storage.host, g_conf.redis_storage.port);
    }
    zlog_debug(g_zc, "[%s] Use redis storage master: [host: %s] [port: %u]", __func__, g_conf.redis_storage.host, g_conf.redis_storage.port);

    return c;
}

redisContext *
create_redis_cache_context(void)
{
    int index = rand() % g_conf.redis_cache_count;
    struct timeval timeout = {0, g_conf.redis_cache_array[index].timeout * 1000};
    redisContext *c = redisConnectWithTimeout(g_conf.redis_cache_array[index].host, g_conf.redis_cache_array[index].port, timeout);
    if (NULL == c || c->err) {
        if (c) {
            redisFree(c);
            c = NULL;
        }
        zlog_warn(g_zc, "Can NOT connect redis cache: [index: %d] [host: %s] [port: %u]", index,
                g_conf.redis_cache_array[index].host, g_conf.redis_cache_array[index].port);
    }
    zlog_debug(g_zc, "[%s] Use redis cache: [index: %d] [host: %s] [port: %u]", __func__, index,
            g_conf.redis_cache_array[index].host, g_conf.redis_cache_array[index].port);

    return c;
}

size_t
get_date_time_str(char *buf, size_t buf_len)
{
    time_t t = time(NULL);
    /** localtime() 返回的是静态指针,不需要 free() */
    return strftime(buf, buf_len, "%Y-%m-%d %H:%M:%S", localtime(&t));
}
