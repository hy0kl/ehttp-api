/**
 * @describe: 基于 libehtp 的接口项目
 * @author: Jerry Yang(hy0kle@gmail.com)
 * @notice: 仅支持 *nix 系统
 * TODO
 * 1. 一期不使用内存池,先出一版看效果
 * 2. redis storage 应该也配置成主从,先不实现.
 * 3. 信号安装.
 * */
#include "util.h"
#include "init.h"
#include "setproctitle.h"

/** 业务逻辑 */
#include "account/demo.h"

/** 全局变量 */
server_config_t   g_conf;
zlog_category_t  *g_zc;

char  **os_argv;

int main(int argc, char *argv[])
{
    os_argv = (char **)argv;
    // 随机数播种
    srand((unsigned)time(NULL));

    init();
    // 日志库开启了
    zlog_info(g_zc, "工作环境初始化成功, Let's work.");

    /** 设置工作进程 title,有利于控制程序部署切换 */
    char prog[CONF_BUF_LEN];
    char *p = argv[0];
    if ('.' == p[0]) { p += 2;}
    snprintf(prog, CONF_BUF_LEN, "%s:%u %s", p, g_conf.port, argv[0]);
    logprintf("prog-title: %s", prog);
    init_setproctitle(argc, argv);
    setproctitle(prog);

    uint64_t max_keepalives = 60;
    evbase_t * evbase = event_base_new();
    evhtp_t  * htp    = evhtp_new(evbase, NULL);

    evhtp_set_max_keepalive_requests(htp, max_keepalives);

    // 注册路由
    evhtp_set_glob_cb(htp, "/account/demo", account_demo, NULL);
    evhtp_set_glob_cb(htp, "*", default_router, NULL);

    evhtp_bind_socket(htp, g_conf.ip, g_conf.port, g_conf.backlog);

    event_base_loop(evbase, 0);

    evhtp_unbind_socket(htp);
    evhtp_free(htp);
    event_base_free(evbase);

    // 清理 clean
    clean();
    return 0;
}

/* vim:set ft=c ts=4 sw=4 et fdm=marker: */

