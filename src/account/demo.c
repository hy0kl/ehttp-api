/**
 * @describe:
 * @author: Jerry Yang(hy0kle@gmail.com)

```sql
DROP TABLE IF EXISTS `demo`;
CREATE TABLE IF NOT EXISTS `demo` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '主键',
  `nickname` varchar(128) NOT NULL,
  `mobile` varchar(32) NOT NULL,
  `email` varchar(64) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=UTF8 ;
```

 * */
#include "demo.h"

void
account_demo(evhtp_request_t *req, void *arg)
{
    log_uri(req);
    set_json_header(req);

#if (_DEBUG_)
    struct timeval start_tv;
    gettimeofday(&start_tv, NULL);
#endif

    g_error_code_e ret_code = API_OK;
    /** 响应体 json 对象 */
    cJSON *root_json = cJSON_CreateObject();
    cJSON *data      = cJSON_CreateObject();

    /** 处理请求参数 */
    req_param_filter_t req_filter_conf[] = {
        {"version", REQUIRED_NO, REQ_PARAM_STRING},
        {"id", REQUIRED_NO, REQ_PARAM_INT},
        //{"test", REQUIRED_YES, REQ_PARAM_STRING},
        REQ_PARAM_FILTER_PAD,
    };
    cJSON *req_filter_data = cJSON_CreateObject();
    ret_code = filter_request_parameters(req, req_filter_conf, req_filter_data);

    if (API_OK != ret_code) { goto FINISH; }

    /** data 子对象 */
    cJSON *array = cJSON_CreateArray();
    cJSON_AddItemToObject(data, "demo_data", array);

    Connection_T db;
    /** 如果觉得代码缩进太深,可以合理使用 goto */
    do {
        ret_code = get_slave_db_link(&db);
        if (API_OK != ret_code) { break; }

        TRY
        {
            ResultSet_T result = Connection_executeQuery(db,
                "SELECT id, nickname, mobile, email FROM demo");
            while (ResultSet_next(result)) {
                cJSON *obj = cJSON_CreateObject();
                cJSON_AddItemToArray(array, obj);

                int id = ResultSet_getIntByName(result, "id");
                cJSON_AddNumberToObject(obj, "id", id);

                const char *nickname = ResultSet_getStringByName(result, "nickname");
                cJSON_AddStringToObject(obj, "nickname", nickname);

                const char *mobile = ResultSet_getStringByName(result, "mobile");
                cJSON_AddStringToObject(obj, "mobile", mobile);

                const char *email = ResultSet_getStringByName(result, "email");
                cJSON_AddStringToObject(obj, "email", email);
            }
        }
        CATCH(SQLException)
        {
            ret_code = SERVICE_UNAVAILABLE;
            zlog_error(g_zc, "SQLException -- %s\n", Exception_frame.message);
        }
        FINALLY
        {
            Connection_close(db);
        }
        END_TRY;
    } while(0);

    const char *api = "http://sug.so.360.cn/suggest?callback=suggest_so&encodein=utf-8&encodeout=utf-8&format=json&fields=word,obdata&word=abc";
    curl_buf_t *curl_buf = create_curl_buf(CURL_CHUNK_BUF_LEN);
    if (NULL == curl_buf) {
        ret_code = NEED_MORE_MEMORY;
        goto FINISH;
    }
    ret_code = curl_get_api(api, curl_buf);
    if (API_OK != ret_code) { goto FINISH; }
    cJSON_AddStringToObject(data, "api_data", curl_buf->buf);

FINISH:

    /** 构建基本包体 */
    build_base_json(root_json, ret_code);
    cJSON_AddItemToObject(root_json, RES_DATA, data);

    //char *json = cJSON_Print(root_json);
    char *json = cJSON_PrintUnformatted(root_json);
    evbuffer_add(req->buffer_out, json, strlen(json));

    /** 清除内存 */
    delete_curl_buf(curl_buf);
    if (json) { free(json); }
    if (root_json) { cJSON_Delete(root_json); }
    if (req_filter_data) { cJSON_Delete(req_filter_data); }

    /** 发送响应 */
    evhtp_send_reply(req, EVHTP_RES_OK);

#if (_DEBUG_)
    struct timeval end_tv;
    gettimeofday(&end_tv, NULL);
    zlog_debug(g_zc, "响应时间: %ld微秒", GETUTIME(end_tv) - GETUTIME(start_tv));
#endif
}
/* vim:set ft=c ts=4 sw=4 et fdm=marker: */

