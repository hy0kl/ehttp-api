#ifndef _CRYPTO_WRAP_H_
#define _CRYPTO_WRAP_H_

#include "server.h"

/** 简单包装 md5() */
char *
md5sum(const char *data, char *md5_buf, const size_t md5_buf_len);
#endif
