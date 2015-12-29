/**
 * @describe: 基于 libehtp 的接口项目
 * @author: Jerry Yang(hy0kle@gmail.com)
 * @notice: 仅支持 *nix 系统
 * 一期不使用内存池,先出一版看效果
 * */
#include "util.h"
#include "init.h"

/** 全局变量 */
server_config_t   g_conf;
zlog_category_t  *g_zc;
ConnectionPool_T  mysql_master_pool;
ConnectionPool_T *mysql_slaves_pool;

int main(int argc, char *argv[])
{
    init();
    printf("api server.\n");
    return 0;
}

/* vim:set ft=c ts=4 sw=4 et fdm=marker: */

