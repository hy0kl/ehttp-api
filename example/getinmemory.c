/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2015, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
/* <DESC>
 * Shows how the write callback function can be used to download data into a
 * chunk of memory instead of storing it in a file.
 * </DESC>
 * gcc -o getinmemory getinmemory.c -lcurl
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#define BUF_LEN 10240

struct MemoryStruct {
    char  *memory;
    size_t size;    /** 已使用的缓冲区大小 */
    size_t buf_len;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    /** 如果缓冲区不够容纳结果,扩充缓冲区 */
    if (realsize + 1 + mem->size > mem->buf_len) {
        mem->memory = realloc(mem->memory, mem->size + realsize + 1);
        if (mem->memory == NULL) {
            /* out of memory! */
            printf("not enough memory (realloc returned NULL)\n");
            return 0;
        }

        mem->buf_len += realsize + 1;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int main(int argc, char *argv[])
{
    CURL *curl_handle;
    CURLcode res;
    long http_code = 0;

    struct MemoryStruct chunk;

    chunk.memory = malloc(BUF_LEN);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    chunk.buf_len = BUF_LEN;

    /** 全局初始化,非线程安全,请在主进程/线程中执行 */
    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, "https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=APPID&secret=APPSECRET");

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    /* some servers don't like requests that are made without a user-agent
       field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* example.com is redirected, so we tell libcurl to follow redirection */ 
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

    /* complete connection within 100 milliseconds */
    curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 100L);

    /* complete within 300 milliseconds */
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, 300L);

    /* get it! */
    res = curl_easy_perform(curl_handle);

    /* check for errors */
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    else {
        /*
         * Now, our chunk.memory points to a memory block that is chunk.size
         * bytes big and contains the remote file.
         *
         * Do something nice with it!
         */
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
        printf("HTTP_CODE: %ld\n", http_code);

        /* ask for the content-type */
        char *ct;
        res = curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_TYPE, &ct);

        if ((CURLE_OK == res) && ct) {
            printf("We received Content-Type: [%s]\n", ct);
        }

        printf("RESULT: %s\n", chunk.memory);
        printf("%lu bytes retrieved\n", (long)chunk.size);
    }

    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);

    free(chunk.memory);

    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();

    return 0;
}

