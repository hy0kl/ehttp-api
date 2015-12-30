#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <err.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

/** 引入依赖库头文件 */
#include <hiredis/hiredis.h>
#include <evhtp.h>
#include <curl/curl.h>
#include <zdb.h>
#include <zlog.h>

#include "../contrib/cjson/cJSON.h"

/**
 _ __ ___   __ _  ___ _ __ ___
| '_ ` _ \ / _` |/ __| '__/ _ \
| | | | | | (_| | (__| | | (_) |
|_| |_| |_|\__,_|\___|_|  \___/
*/
#define _DEBUG_         1

#if (_DEBUG_) /** { */
#define logprintf(format, arg...) fprintf(stderr, "[DEBUG]%s:%d:%s "format"\n", __FILE__, __LINE__, __func__, ##arg)
#else /** } {*/
#define logprintf(format, arg...) {}
#endif /** } */

/** 配置项目缓冲区长度 */
#define CONF_BUF_LEN    128
/** 文件路径缓冲区长度 */
#define PATH_BUF_LEN    1024

#define SMALL_BUF_LEN       8
#define CURL_CHUNK_BUF_LEN  10240
#define CURL_TIMEOUT        450

#define GETUTIME(t) ((t.tv_sec) * 1000000 + (t.tv_usec))
#define GETSTIME(t) (t.tv_sec)
#define GETMTIME(t) ((((t.tv_sec) * 1000000 + (t.tv_usec))) / 1000)

#define MYSQL_PORT      3306
#define REDIS_PORT      6379
#define REDIT_TIMEOUT   100

/* Port to listen on. */
#define SERVER_PORT     5678
#define SERVER_BACKLOG  1024

/** 全局状态码/错误码 */
typedef enum _g_error_code_e
{
    CAN_NOT_OPEN_SERVER_JSON    = -11,
    NEED_MORE_MEMORY            = -12,
    JSON_PARSE_FAILURE          = -13,
    CAN_NOT_OPEN_ZLOG_CONF      = -1,
    CAN_NOT_GET_ZLOG_CATEGORY   = -2,

    LOST_MYSQL_CONFIG           = 400100,
    LOST_MYSQL_MASTER_CONFIG,
    LOST_MYSQL_MASTER_HOST,
    LOST_MYSQL_MASTER_DBNAME,
    LOST_MYSQL_MASTER_USERNAME,
    LOST_MYSQL_MASTER_PASSWORD,
    LOST_MYSQL_SLAVES_CONFIG,
    LOST_MYSQL_SLAVES_HOST,
    LOST_MYSQL_SLAVES_DBNAME,
    LOST_MYSQL_SLAVES_USERNAME,
    LOST_MYSQL_SLAVES_PASSWORD,

    LOST_REDIS_CONFIG,
    LOST_REDIS_STORAGE,
    LOST_REDIS_STORAGE_HOST,
    LOST_REDIS_CACHE,
    LOST_REDIS_CACHE_HOST,

    LOST_CURL_CONFIG,
} g_error_code_e;

/** 全局配置*/
typedef unsigned int u_int;
// mysql 节点
typedef struct _mysql_config_t
{
    char   host[CONF_BUF_LEN];
    u_int  port;
    char   dbname[CONF_BUF_LEN];
    char   username[CONF_BUF_LEN];
    char   password[CONF_BUF_LEN];

    /** 连接池的 dns url*/
    URL_T            url;
    ConnectionPool_T pool;
} mysql_config_t;

// redis 节点
typedef struct _redis_config_t
{
    char  host[CONF_BUF_LEN];
    u_int port;
    u_int timeout;  /** 超时时间,单位毫秒 */
} redis_config_t;

// curl 配置
typedef struct _curl_config_t
{
    // curl 的超时设置
    u_int chunk_buf_len;
    long  connect_timeout_ms;
    long  timeout_ms;
} curl_config_t;

typedef struct _server_config_t
{
    char  env[SMALL_BUF_LEN];
    char  ip[CONF_BUF_LEN];
    u_int port;
    u_int backlog;

    char zlog_conf[CONF_BUF_LEN];
    char zlog_category[CONF_BUF_LEN];

    mysql_config_t  mysql_master; /** 主节点只有一个 */
    u_int           mysql_slaves_count;
    mysql_config_t *mysql_slaves_array; /** 从库是一组 */

    // redis 不考先不考虑主从,失败重试等复杂情况
    redis_config_t  redis_storage;  /** redis 存储是单点 */
    u_int           redis_cache_count;
    redis_config_t *redis_cache_array;  /** 缓存是一组服务 */

    curl_config_t   curl_conf;
} server_config_t;

/** 全局变量 */
extern server_config_t   g_conf;
extern zlog_category_t  *g_zc;

// redis 不需要链接池
#endif
