#include <stdio.h>
#include <zlog.h>

/* zlog macros */
//zlog_fatal(cat, format, ...)
//zlog_error(cat, format, ...)
//zlog_warn(cat, format, ...)
//zlog_notice(cat, format, ...)
//zlog_info(cat, format, ...)
//zlog_debug(cat, format, ...)

int main(int argc, char *argv[])
{
    int rc;
    zlog_category_t *c;

    rc = zlog_init("./zlog.conf");
    if (rc) {
        printf("init failed, can not find *.conf file\n");
        return -1;
    }

    c = zlog_get_category("my_cat");
    if (!c) {
        printf("get cat fail\n");
        zlog_fini();
        return -2;
    }

    zlog_info(c, "hello, this zlog. INFO log");
    zlog_warn(c, "warning log.");
    zlog_debug(c, "debug, zlog DEBUG log");

    zlog_fini();

    return 0;
}
