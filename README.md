# ehttp-api
http libevent api service

# Dependencies

- libevent http://libevent.org/
- libcurl http://curl.haxx.se/
- hiredis https://github.com/redis/hiredis
- libevhtp https://github.com/ellzey/libevhtp
- cJSON https://github.com/kbranigan/cJSON http://cjson.sourceforge.net/
- libzdb https://github.com/mverbert/libzdb http://www.tildeslash.com/libzdb
- zlog http://hardysimpson.github.com/zlog/

# install guide

Except cJSON:

```
$ wget DPND
$ tar xf DPND.tar*
$ cd DPND
$ ./configure # Optional, if it has
$ make
$ sudo make install

# libzdb 安装后的特殊处理
$ cd /usr/local/include/zdb
$ sudo cp * ../
```
