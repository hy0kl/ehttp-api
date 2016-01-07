#ifndef _CRYPTO_WRAP_H_
#define _CRYPTO_WRAP_H_

#include "server.h"

/**
 * 简单包装 md5()
 * 返回 md5_buf 的指针.或许返回 void 更合适,只是为了签名上与 MD5 相一致
 * */
char *
md5sum(const char *data, char *md5_buf, const size_t md5_buf_len);
#endif
