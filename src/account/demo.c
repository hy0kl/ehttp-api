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
    g_error_code_e ret_code = API_OK;
    struct timeval start_tv;
    gettimeofday(&start_tv, NULL);

    log_uri(req);
    set_json_header(req);

    /** 处理请求参数 */
    req_param_filter_t req_filter_conf[] = {
        {"version", REQUIRED_NO, REQ_PARAM_STRING},
        {"id", REQUIRED_NO, REQ_PARAM_INT},
        REQ_PARAM_FILTER_PAD,
    };
    cJSON *req_filter_data = cJSON_CreateObject();
    ret_code = filter_request_parameters(req, req_filter_conf, req_filter_data);

    /** 响应体 json 对象 */
    cJSON *root_json = cJSON_CreateObject();

    cJSON *data  = cJSON_CreateObject();
    cJSON *array = cJSON_CreateArray();
    cJSON_AddItemToObject(data, "demo_data", array);

    Connection_T db;
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

    /** 构建基本包体 */
    build_base_json(root_json, ret_code);
    cJSON_AddItemToObject(root_json, RES_DATA, data);

    //char *json = cJSON_Print(root_json);
    char *json = cJSON_PrintUnformatted(root_json);
    evbuffer_add(req->buffer_out, json, strlen(json));

    // 清除内存
    if (json) { free(json); }
    if (root_json) { cJSON_Delete(root_json); }
    if (req_filter_data) { cJSON_Delete(req_filter_data); }

    evhtp_send_reply(req, EVHTP_RES_OK);

    struct timeval end_tv;
    gettimeofday(&end_tv, NULL);
#if (_DEBUG_)
    zlog_debug(g_zc, "响应时间: %ld微秒", GETUTIME(end_tv) - GETUTIME(start_tv));
#endif
}
/* vim:set ft=c ts=4 sw=4 et fdm=marker: */

