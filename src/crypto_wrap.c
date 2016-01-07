/**
 * @describe: 加密算法的简单包装
 * @author: Jerry Yang(hy0kle@gmail.com)
 * */

#include "crypto_wrap.h"

char *
md5sum(const char *data, char *md5_buf, const size_t md5_buf_len)
{
    assert(data);
    assert(md5_buf);
    assert(md5_buf_len > 32);

    int i;

    char tmp[3] = {0};

    unsigned char md[16];

    MD5((unsigned char *)data, strlen(data), md);

    for (i = 0; i < 16; i++){
        sprintf(tmp, "%2.2x", md[i]);
        strcat(md5_buf, tmp);
    }

    return md5_buf;
}

/* vim:set ft=c ts=4 sw=4 et fdm=marker: */

