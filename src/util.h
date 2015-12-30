#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>

#include "server.h"

/** 全局的 code message */
const char *
get_message(g_error_code_e code);

/** 从连接池获取主库连接 */
g_error_code_e
get_master_db_link(Connection_T *db_link);

/** 从连接池中取从库连接 */
g_error_code_e
get_slave_db_link(Connection_T *db_link);

/** 打印 access 日志 */
void
log_uri(evhtp_request_t *req);

/** 默认路由 */
void
default_router(evhtp_request_t *req, void *arg);

void clean(void);
#endif

