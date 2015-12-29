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
    int port = cJSON_GetObjectItem(root_json, "port")->valueint;
    if (port > 0) {
        g_conf.port = (u_int)port;
    }
    logprintf("g_conf.port = %d", g_conf.port);

    // backlog
    g_conf.backlog = SERVER_BACKLOG;
    int backlog = cJSON_GetObjectItem(root_json, "backlog")->valueint;
    if (backlog > 0) {
        g_conf.backlog = (u_int)backlog;
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
    cJSON *host = cJSON_GetObjectItem(mysql_master, "host");
    if (NULL != host) {
        snprintf(g_conf.mysql_master.host, CONF_BUF_LEN, "%s", host->valuestring);
    } else {
        fprintf(stderr, "%s\n", get_message(LOST_MYSQL_MASTER_HOST));
        cJSON_Delete(root_json);
        exit(LOST_MYSQL_MASTER_HOST);
    }
    logprintf("g_conf.mysql_master.host: %s", g_conf.mysql_master.host);
    g_conf.mysql_master.port = MYSQL_PORT;
    port = cJSON_GetObjectItem(mysql_master, "port")->valueint;
    if (port > 0) {
        g_conf.mysql_master.port = (u_int)port;
    }
    logprintf("g_conf.mysql_master.port: %u", g_conf.mysql_master.port);
    cJSON *dbname = cJSON_GetObjectItem(mysql_master, "dbname");
    if (NULL != dbname) {
        snprintf(g_conf.mysql_master.dbname, CONF_BUF_LEN, "%s", dbname->valuestring);
    } else {
        fprintf(stderr, "%s\n", get_message(LOST_MYSQL_MASTER_DBNAME));
        cJSON_Delete(root_json);
        exit(LOST_MYSQL_MASTER_DBNAME);
    }
    logprintf("g_conf.mysql_master.dbname: %s", g_conf.mysql_master.dbname);
    cJSON *username = cJSON_GetObjectItem(mysql_master, "username");
    if (NULL != username) {
        snprintf(g_conf.mysql_master.username, CONF_BUF_LEN, "%s", username->valuestring);
    } else {
        fprintf(stderr, "%s\n", get_message(LOST_MYSQL_MASTER_USERNAME));
        cJSON_Delete(root_json);
        exit(LOST_MYSQL_MASTER_USERNAME);
    }
    logprintf("g_conf.mysql_master.username: %s", g_conf.mysql_master.username);
    cJSON *password = cJSON_GetObjectItem(mysql_master, "password");
    if (NULL != password) {
        snprintf(g_conf.mysql_master.password, CONF_BUF_LEN, "%s", password->valuestring);
    } else {
        fprintf(stderr, "%s\n", get_message(LOST_MYSQL_MASTER_PASSWORD));
        cJSON_Delete(root_json);
        exit(LOST_MYSQL_MASTER_PASSWORD);
    }
    logprintf("g_conf.mysql_master.password: %s", g_conf.mysql_master.password);

    cJSON_Delete(root_json);
    free(data);

    return;
}

static void
init_global()
{
    logprintf("init global");
}

void init()
{
    parse_server_config();
    init_global();
}
