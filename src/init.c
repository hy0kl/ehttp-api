#include "init.h"

static void
parse_server_config()
{
    logprintf("parse");
    FILE *fp = fopen("conf/server.json", "r");
    if (NULL == fp) {
        fprintf(stderr, "无法打开主配置文件: conf/server.json\n");
        exit(CAN_NOT_OPEN_SERVER_JSON);
    }

    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *data = (char*)malloc(len + 1);

    if (NULL == data) {
        fprintf(stderr, "无法为解析配置文件申请足够的内存. need: %ld\n", len);
        exit(NEED_MORE_MEMORY);
    }

    fread(data, 1, len, fp);
    fclose(fp);

    /** 解析 json 的配置文件 */
    cJSON *root_json = cJSON_Parse(data);
    if (NULL == root_json) {
        fprintf(stderr, "解析JSON失败, error:%s\n", cJSON_GetErrorPtr());

        cJSON_Delete(root_json);
        free(data);

        exit(JSON_PARSE_FAILURE);
    }

    // env
    cJSON *env = cJSON_GetObjectItem(root_json, "RUNTIME_ENVIROMENT");
    if (NULL != env) {
        snprintf(g_conf.env, SMALL_BUF_LEN, "%s", env->valuestring);
    } else {
        snprintf(g_conf.env, SMALL_BUF_LEN, "%s", "UNKNOWN");
    }
    logprintf("g_conf.env: %s", g_conf.env);

    // ip
    cJSON *ip = cJSON_GetObjectItem(root_json, "ip");
    if (NULL != ip) {
        snprintf(g_conf.ip, CONF_BUF_LEN, "%s", ip->valuestring);
    } else {
        snprintf(g_conf.ip, CONF_BUF_LEN, "127.0.0.1");
    }
    logprintf("g_conf.ip: %s", g_conf.ip);

    // port
    g_conf.port = SERVER_PORT;  /** 默认监听端口 */
    cJSON *port = cJSON_GetObjectItem(root_json, "port");
    if (port && port->valueint > 0) {
        g_conf.port = port->valueint;
    }
    logprintf("g_conf.port = %d", g_conf.port);

    // backlog
    g_conf.backlog = SERVER_BACKLOG;
    cJSON *backlog = cJSON_GetObjectItem(root_json, "backlog");
    if (backlog && backlog->valueint > 0) {
        g_conf.backlog = backlog->valueint;
    }
    logprintf("g_conf.backlog: %u", g_conf.backlog);

    // zlog_conf
    cJSON *zlog_conf = cJSON_GetObjectItem(root_json, "zlog_conf");
    if (NULL != zlog_conf) {
        snprintf(g_conf.zlog_conf, CONF_BUF_LEN, "%s", zlog_conf->valuestring);
    } else {
        snprintf(g_conf.zlog_conf, CONF_BUF_LEN, "./conf/zlog.conf");
    }
    logprintf("g_conf.zlog_conf = %s", g_conf.zlog_conf);

    // zlog_category
    cJSON *zlog_category = cJSON_GetObjectItem(root_json, "zlog_category");
    if (NULL != zlog_category) {
        snprintf(g_conf.zlog_category, CONF_BUF_LEN, "%s", zlog_category->valuestring);
    } else {
        snprintf(g_conf.zlog_category, CONF_BUF_LEN, "default_cat");
    }
    logprintf("g_conf.zlog_category = %s", g_conf.zlog_category);

    // mysql
    cJSON *mysql = cJSON_GetObjectItem(root_json, "mysql");
    if (NULL == mysql) {
        fprintf(stderr, "%s\n", get_message(LOST_MYSQL_CONFIG));
        cJSON_Delete(root_json);
        exit(LOST_MYSQL_CONFIG);
    }

    // master
    cJSON *mysql_master = cJSON_GetObjectItem(mysql, "master");
    if (NULL == mysql_master) {
        fprintf(stderr, "%s\n", get_message(LOST_MYSQL_MASTER_CONFIG));
        cJSON_Delete(root_json);
        exit(LOST_MYSQL_MASTER_CONFIG);
    }
    //// host
    cJSON *host = cJSON_GetObjectItem(mysql_master, "host");
    if (NULL != host) {
        snprintf(g_conf.mysql_master.host, CONF_BUF_LEN, "%s", host->valuestring);
    } else {
        fprintf(stderr, "%s\n", get_message(LOST_MYSQL_MASTER_HOST));
        cJSON_Delete(root_json);
        exit(LOST_MYSQL_MASTER_HOST);
    }
    logprintf("g_conf.mysql_master.host: %s", g_conf.mysql_master.host);
    //// port
    g_conf.mysql_master.port = MYSQL_PORT;
    port = cJSON_GetObjectItem(mysql_master, "port");
    if (port && port->valueint > 0) {
        g_conf.mysql_master.port = port->valueint;
    }
    logprintf("g_conf.mysql_master.port: %u", g_conf.mysql_master.port);
    //// dbname
    cJSON *dbname = cJSON_GetObjectItem(mysql_master, "dbname");
    if (NULL != dbname) {
        snprintf(g_conf.mysql_master.dbname, CONF_BUF_LEN, "%s", dbname->valuestring);
    } else {
        fprintf(stderr, "%s\n", get_message(LOST_MYSQL_MASTER_DBNAME));
        cJSON_Delete(root_json);
        exit(LOST_MYSQL_MASTER_DBNAME);
    }
    logprintf("g_conf.mysql_master.dbname: %s", g_conf.mysql_master.dbname);
    //// username
    cJSON *username = cJSON_GetObjectItem(mysql_master, "username");
    if (NULL != username) {
        snprintf(g_conf.mysql_master.username, CONF_BUF_LEN, "%s", username->valuestring);
    } else {
        fprintf(stderr, "%s\n", get_message(LOST_MYSQL_MASTER_USERNAME));
        cJSON_Delete(root_json);
        exit(LOST_MYSQL_MASTER_USERNAME);
    }
    logprintf("g_conf.mysql_master.username: %s", g_conf.mysql_master.username);
    //// password
    cJSON *password = cJSON_GetObjectItem(mysql_master, "password");
    if (NULL != password) {
        snprintf(g_conf.mysql_master.password, CONF_BUF_LEN, "%s", password->valuestring);
    } else {
        fprintf(stderr, "%s\n", get_message(LOST_MYSQL_MASTER_PASSWORD));
        cJSON_Delete(root_json);
        exit(LOST_MYSQL_MASTER_PASSWORD);
    }
    logprintf("g_conf.mysql_master.password: %s", g_conf.mysql_master.password);

    // mysql_slaves
    cJSON *mysql_slaves = cJSON_GetObjectItem(mysql, "slaves");
    int mysql_slaves_count = cJSON_GetArraySize(mysql_slaves);
    if (mysql_slaves_count <= 0) {
        fprintf(stderr, "%s\n", get_message(LOST_MYSQL_SLAVES_CONFIG));
        cJSON_Delete(root_json);
        exit(LOST_MYSQL_SLAVES_CONFIG);
    }
    g_conf.mysql_slaves_count = mysql_slaves_count;
    logprintf("g_conf.mysql_slaves_count: %d", g_conf.mysql_slaves_count);
    g_conf.mysql_slaves_array = (mysql_config_t *)malloc(sizeof(mysql_config_t) * g_conf.mysql_slaves_count);
    if (NULL == g_conf.mysql_slaves_array) {
        fprintf(stderr, "g_conf.mysql_slaves_array %s\n", get_message(NEED_MORE_MEMORY));
        cJSON_Delete(root_json);
        exit(NEED_MORE_MEMORY);
    }
    int i = 0;
    for (; i < g_conf.mysql_slaves_count; i++) {
        cJSON *slave_item = cJSON_GetArrayItem(mysql_slaves, i);
        //// host
        host = cJSON_GetObjectItem(slave_item, "host");
        if (NULL != host) {
            snprintf(g_conf.mysql_slaves_array[i].host, CONF_BUF_LEN, "%s", host->valuestring);
        } else {
            fprintf(stderr, "index:%d, %s\n", i, get_message(LOST_MYSQL_SLAVES_HOST));
            cJSON_Delete(root_json);
            exit(LOST_MYSQL_SLAVES_HOST);
        }
        logprintf("g_conf.mysql_slaves_array[%d].host: %s", i, g_conf.mysql_slaves_array[i].host);
        //// port
        g_conf.mysql_slaves_array[i].port = MYSQL_PORT;
        port = cJSON_GetObjectItem(slave_item, "port");
        if (port && port->valueint > 0) {
            g_conf.mysql_slaves_array[i].port = port->valueint;
        }
        logprintf("g_conf.mysql_slaves_array[%d].port: %u", i, g_conf.mysql_slaves_array[i].port);
        //// dbname
        dbname = cJSON_GetObjectItem(slave_item, "dbname");
        if (NULL != dbname) {
            snprintf(g_conf.mysql_slaves_array[i].dbname, CONF_BUF_LEN, "%s", dbname->valuestring);
        } else {
            fprintf(stderr, "index: %d, %s\n", i, get_message(LOST_MYSQL_SLAVES_DBNAME));
            cJSON_Delete(root_json);
            exit(LOST_MYSQL_SLAVES_DBNAME);
        }
        logprintf("g_conf.mysql_slaves_array[%d].dbname: %s", i, g_conf.mysql_slaves_array[i].dbname);
        //// username
        username = cJSON_GetObjectItem(slave_item, "username");
        if (NULL != username) {
            snprintf(g_conf.mysql_slaves_array[i].username, CONF_BUF_LEN, "%s", username->valuestring);
        } else {
            fprintf(stderr, "index: %d, %s\n", i, get_message(LOST_MYSQL_SLAVES_USERNAME));
            cJSON_Delete(root_json);
            exit(LOST_MYSQL_SLAVES_USERNAME);
        }
        logprintf("g_conf.mysql_slaves_array[%d].username: %s", i, g_conf.mysql_slaves_array[i].username);
        //// password
        password = cJSON_GetObjectItem(slave_item, "password");
        if (NULL != password) {
            snprintf(g_conf.mysql_slaves_array[i].password, CONF_BUF_LEN, "%s", password->valuestring);
        } else {
            fprintf(stderr, "index: %d, %s\n", i, get_message(LOST_MYSQL_SLAVES_PASSWORD));
            cJSON_Delete(root_json);
            exit(LOST_MYSQL_SLAVES_PASSWORD);
        }
        logprintf("g_conf.mysql_slaves_array[%d].password: %s", i, g_conf.mysql_slaves_array[i].password);
    }

    // redis
    cJSON *redis = cJSON_GetObjectItem(root_json, "redis");
    if (NULL == redis) {
        fprintf(stderr, "%s\n", get_message(LOST_REDIS_CONFIG));
        cJSON_Delete(root_json);
        exit(LOST_REDIS_CONFIG);
    }
    /// storage
    cJSON *redis_storage = cJSON_GetObjectItem(redis, "storage");
    if (NULL == redis_storage) {
        fprintf(stderr, "%s\n", get_message(LOST_REDIS_STORAGE));
        cJSON_Delete(root_json);
        exit(LOST_REDIS_STORAGE);
    }
    //// host
    host = cJSON_GetObjectItem(redis_storage, "host");
    if (NULL != host) {
        snprintf(g_conf.redis_storage.host, CONF_BUF_LEN, "%s", host->valuestring);
    } else {
        fprintf(stderr, "%s\n", get_message(LOST_REDIS_STORAGE));
        cJSON_Delete(root_json);
        exit(LOST_REDIS_STORAGE_HOST);
    }
    logprintf("g_conf.redis_storage.host: %s", g_conf.redis_storage.host);
    //// port
    g_conf.redis_storage.port = REDIS_PORT;
    port = cJSON_GetObjectItem(redis_storage, "port");
    if (port && port->valueint > 0) {
        g_conf.redis_storage.port = port->valueint;
    }
    logprintf("g_conf.redis_storage.port: %d", g_conf.redis_storage.port);
    //// timeout
    g_conf.redis_storage.timeout = REDIT_TIMEOUT;
    cJSON *timeout = cJSON_GetObjectItem(redis_storage, "timeout");
    if (timeout && timeout->valueint > 0) {
        g_conf.redis_storage.timeout = timeout->valueint;
    }
    logprintf("g_conf.redis_storage.timeout: %dms", g_conf.redis_storage.timeout);
    /// cache
    cJSON *redis_cache = cJSON_GetObjectItem(redis, "cache");
    int redis_cache_count = cJSON_GetArraySize(redis_cache);
    if (redis_cache_count <= 0) {
        fprintf(stderr, "%s", get_message(LOST_REDIS_CACHE));
        cJSON_Delete(root_json);
        exit(LOST_REDIS_CACHE);
    }
    g_conf.redis_cache_count = redis_cache_count;
    logprintf("g_conf.redis_cache_count: %d", g_conf.redis_cache_count);
    g_conf.redis_cache_array = (redis_config_t *)malloc(sizeof(redis_config_t) * g_conf.redis_cache_count);
    if (NULL == g_conf.redis_cache_array) {
        fprintf(stderr, "g_conf.redis_cache_array %s\n", get_message(NEED_MORE_MEMORY));
        cJSON_Delete(root_json);
        exit(NEED_MORE_MEMORY);
    }
    for (i = 0; i < g_conf.redis_cache_count; i++) {
        cJSON * cache_item = cJSON_GetArrayItem(redis_cache, i);
        //// host
        host = cJSON_GetObjectItem(cache_item, "host");
        if (NULL != host) {
            snprintf(g_conf.redis_cache_array[i].host, CONF_BUF_LEN, "%s", host->valuestring);
        } else {
            fprintf(stderr, "index: %d, %s\n", i, get_message(LOST_REDIS_CACHE_HOST));
            cJSON_Delete(root_json);
            exit(LOST_REDIS_CACHE_HOST);
        }
        logprintf("g_conf.redis_cache_array[%d].host: %s", i, g_conf.redis_cache_array[i].host);
        //// port
        g_conf.redis_cache_array[i].port = REDIS_PORT;
        port = cJSON_GetObjectItem(cache_item, "port");
        if (port && port->valueint > 0) {
            g_conf.redis_cache_array[i].port = port->valueint;
        }
        logprintf("g_conf.redis_cache_array[%d].port: %u", i, g_conf.redis_cache_array[i].port);
        //// timeout
        g_conf.redis_cache_array[i].timeout = REDIT_TIMEOUT;
        timeout = cJSON_GetObjectItem(cache_item, "timeout");
        if (timeout && timeout->valueint > 0) {
            g_conf.redis_cache_array[i].timeout = timeout->valueint;
        }
        logprintf("g_conf.redis_cache_array[%d].timeout: %u", i, g_conf.redis_cache_array[i].timeout);
    }

    // curl
    cJSON *curl = cJSON_GetObjectItem(root_json, "curl");
    if (NULL == curl) {
        fprintf(stderr, "%s\n", get_message(LOST_CURL_CONFIG));
        cJSON_Delete(root_json);
        exit(LOST_CURL_CONFIG);
    }
    // chunk_buf_len
    g_conf.curl_conf.chunk_buf_len = CURL_CHUNK_BUF_LEN;
    cJSON *chunk_buf_len = cJSON_GetObjectItem(curl, "chunk_buf_len");
    if (chunk_buf_len && chunk_buf_len->valueint > CURL_CHUNK_BUF_LEN) {
        g_conf.curl_conf.chunk_buf_len = chunk_buf_len->valueint;
    }
    logprintf("g_conf.curl_conf.chunk_buf_len: %d", g_conf.curl_conf.chunk_buf_len);
    // connect_timeout_ms
    g_conf.curl_conf.connect_timeout_ms = REDIT_TIMEOUT;
    cJSON *connect_timeout_ms = cJSON_GetObjectItem(curl, "connect_timeout_ms");
    if (connect_timeout_ms && connect_timeout_ms->valueint > 0) {
        g_conf.curl_conf.connect_timeout_ms = connect_timeout_ms->valueint;
    }
    logprintf("g_conf.curl_conf.connect_timeout_ms: %ldms", g_conf.curl_conf.connect_timeout_ms);
    // timeout_ms
    g_conf.curl_conf.timeout_ms = REDIT_TIMEOUT;
    cJSON *timeout_ms = cJSON_GetObjectItem(curl, "timeout_ms");
    if (timeout_ms && timeout_ms->valueint > 0) {
        g_conf.curl_conf.timeout_ms = timeout_ms->valueint;
    }
    logprintf("g_conf.curl_conf.timeout_ms: %ldms", g_conf.curl_conf.timeout_ms);

    cJSON_Delete(root_json);
    free(data);

    return;
}

static void
init_global()
{
    logprintf("init global");
    // 初始化日志库
    int rc;
    rc = zlog_init(g_conf.zlog_conf);
    if (rc) {
        fprintf(stderr, "init failed, can not find %s file\n", g_conf.zlog_conf);
        exit(CAN_NOT_OPEN_ZLOG_CONF);
    }

    g_zc = zlog_get_category(g_conf.zlog_category);
    if (!g_zc) {
        fprintf(stderr, "%s\n", get_message(CAN_NOT_GET_ZLOG_CATEGORY));
        zlog_fini();
        exit(CAN_NOT_GET_ZLOG_CATEGORY);
    }

    zlog_debug(g_zc, "日志库初始化成功");

    // 初始化数据库链接池
    char url_buf[CONF_BUF_LEN];
    snprintf(url_buf, CONF_BUF_LEN, "mysql://%s:%d/%s?user=%s&password=%s",
            g_conf.mysql_master.host, g_conf.mysql_master.port,
            g_conf.mysql_master.dbname, g_conf.mysql_master.username,
            g_conf.mysql_master.password);
    logprintf("mysql.master.dns-url: %s", url_buf);

    g_conf.mysql_master.url  = URL_new(url_buf);
    g_conf.mysql_master.pool = ConnectionPool_new(g_conf.mysql_master.url);
    ConnectionPool_start(g_conf.mysql_master.pool);

    zlog_debug(g_zc, "数据库连接池初始化成功");
}

void init()
{
    parse_server_config();
    init_global();
}
