# ehttp-api
http libevent api service

# Dependencies

## 以下需要安装
- libevent http://libevent.org/
- libcurl http://curl.haxx.se/
- hiredis https://github.com/redis/hiredis
- libevhtp https://github.com/ellzey/libevhtp
- libzdb https://github.com/mverbert/libzdb http://www.tildeslash.com/libzdb
- zlog http://hardysimpson.github.com/zlog/
- TCMalloc https://github.com/gperftools/gperftools
- openssl https://github.com/openssl/openssl

## 以下不需要安装
- cJSON https://github.com/kbranigan/cJSON http://cjson.sourceforge.net/
- nginx 的部分源码

# install guide

Except cJSON:

```
$ wget DPND
$ tar xf DPND.tar*
$ cd DPND
$ ./configure # Optional, if it has
$ make
$ sudo make install

# libevhtp 的特殊处理
由于内网的 http 服务,可以将 openssl 禁掉.
$ cd libevhtp-*
$ vim evhtp.h
加入以下内容:
#define EVHTP_DISABLE_SSL 1

安装成功后执行:
$ cd /usr/local/include/evhtp
$ sudo cp * ../

# libzdb 安装后的特殊处理
$ cd /usr/local/include/zdb
$ sudo cp * ../
```

# 注意事项

新 clone 的项目请依次执行以下的命令:

```
$ git submodule init
$ git submodule update
```

***需要参考 conf 目录中 *.sample.conf 配置生成开发或线上环境服务配置文件.***

```
$ cp conf/server.sample.json conf/server.json
$ cp conf/zlog.sample.conf conf/zlog.conf
将配置文件中修改为真实可用的资源.
```

# Q&A

## Mac 下报错: `dyld: Library not loaded: libmysqlclient.18.dylib`

```
参考方法:
$ sudo ln -s /usr/local/mysql/lib/libmysqlclient.18.dylib /usr/local/lib/libmysqlclient.18.dylib
```

## bigint 如何处理?

传输出层采用字符串,业务使用的时候转成数字.

```c
#include <stdlib.h>

long
strtol(const char *restrict str, char **restrict endptr, int base);

long long
strtoll(const char *restrict str, char **restrict endptr, int base);

unsigned long
strtoul(const char *restrict str, char **restrict endptr, int base);

unsigned long long
strtoull(const char *restrict str, char **restrict endptr, int base);
```

## 如何防SQL注入

1. 优先考虑使用 `Connection_prepareStatement`.
2. 其次,由于客户端提交数据经过 filter 处理后,变成了 cJSON 对象,会对字符串中的双引号加上转义符,如果是手工拼 SQL,只要用双引号引起字符串即可防注入.形如:

    ```c
    ResultSet_T result = Connection_executeQuery(db, "SELECT id, nickname, mobile, email FROM demo WHERE email = \"%s\"", email->valuestring);
    ```

