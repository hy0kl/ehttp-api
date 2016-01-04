#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>

#include "server.h"

#define SIGNO_END       111

#define REQUIRED_YES    1
#define REQUIRED_NO     0

/** 参数过滤相关 简单处理,仅支持两种基本类型 */
typedef enum _request_parameter_type_e
{
    REQ_PARAM_STRING = 0,
    REQ_PARAM_INT, // int
    //REQ_PARAM_BOOL,
} req_param_type_e;

typedef struct _request_parameter_filter_t
{
    char *param;
    int   required; /** 是否是必须的 */
    req_param_type_e type;  /** 参数的类型 */
} req_param_filter_t;

#define REQ_PARAM_FILTER_PAD    {NULL, REQUIRED_NO, REQ_PARAM_STRING}

typedef struct _curl_buf_t
{
    char  *buf;
    size_t size;    /** 已使用的缓冲区大小 */
    size_t buf_len; /** buf 的长度 */
} curl_buf_t;

/** 安装信号 */
void signal_setup(void);

/** 全局的 code message */
const char *
get_message(const g_error_code_e code);

/** 从连接池获取主库连接 */
g_error_code_e
get_master_db_link(Connection_T *db_link);

/** 从连接池中取从库连接 */
g_error_code_e
get_slave_db_link(Connection_T *db_link);

/** 设置 json 头 */
void set_json_header(evhtp_request_t *req);

/** 打印 access 日志 */
void log_uri(const evhtp_request_t *req);

/** 构造 base json 对象的结构 */
void build_base_json(cJSON *root_json, g_error_code_e code);

/** 默认路由 */
void default_router(evhtp_request_t *req, void *arg);

/** 取得原始的 POST 数据 */
g_error_code_e
get_post_data_raw(const evhtp_request_t *req, char *buf, size_t buf_len);

/** 退出时清理所占资源 */
void clean(void);

/** 通用过滤客户端请求数据 */
g_error_code_e
filter_request_parameters(const evhtp_request_t *req, const req_param_filter_t *filter, cJSON *json_obj);

/** 创建 curl 工作缓冲区 */
curl_buf_t *
create_curl_buf(size_t buf_len);

/** 释放 curl 的缓冲区 */
void delete_curl_buf(curl_buf_t *curl_buf);
#endif

