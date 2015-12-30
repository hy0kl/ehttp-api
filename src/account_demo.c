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
#include "account_demo.h"

void
account_demo(evhtp_request_t *req, void *arg)
{
    log_uri(req);
    set_json_header(req);

    cJSON *root_json = cJSON_CreateObject();

    cJSON_AddNumberToObject(root_json, RES_CODE, API_OK);
    cJSON_AddStringToObject(root_json, RES_MSG, get_message(API_OK));

    cJSON *data = cJSON_CreateObject();
    cJSON_AddItemToObject(root_json, RES_DATA, data);

    cJSON *array = cJSON_CreateArray();
    cJSON_AddItemToObject(data, "demo_data", array);

    Connection_T db;
    get_slave_db_link(&db);

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
        zlog_error(g_zc, "SQLException -- %s\n", Exception_frame.message);
    }
    FINALLY
    {
        Connection_close(db);
    }
    END_TRY;

    //char *json = cJSON_Print(root_json);
    char *json = cJSON_PrintUnformatted(root_json);
    evbuffer_add(req->buffer_out, json, strlen(json));

    // 清除内存
    if (json) { free(json); }
    if (root_json) { cJSON_Delete(root_json); }

    evhtp_send_reply(req, EVHTP_RES_OK);
}
/* vim:set ft=c ts=4 sw=4 et fdm=marker: */

