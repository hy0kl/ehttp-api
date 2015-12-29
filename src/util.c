#include "util.h"

const char *
get_message(g_error_code_e code)
{
    const char *msg = NULL;
    switch (code) {
        case CAN_NOT_OPEN_SERVER_JSON:
            msg = "Can not open server.json file.";
            break;

        case NEED_MORE_MEMORY:
            msg = "Out of memory.";
            break;

        case JSON_PARSE_FAILURE:
            msg = "json file paser error, please check it out.";
            break;

        case CAN_NOT_OPEN_ZLOG_CONF:
            msg = "Can not open zlog.conf";
            break;

        case CAN_NOT_GET_ZLOG_CATEGORY:
            msg = "Can not get zlog category, please check it out.";
            break;

        case LOST_MYSQL_CONFIG:
            msg = "Lost [mysql] config.";
            break;

        case LOST_MYSQL_MASTER_CONFIG:
            msg = "Lost [mysql.master] config.";
            break;

        case LOST_MYSQL_MASTER_HOST:
            msg = "Lost [mysql.master.host] config.";
            break;

        case LOST_MYSQL_MASTER_DBNAME:
            msg = "Lost [mysql.master.dbname] config.";
            break;

        case LOST_MYSQL_MASTER_USERNAME:
            msg = "Lost [mysql.master.username] config.";
            break;

        case LOST_MYSQL_MASTER_PASSWORD:
            msg = "Lost [mysql.master.password] config.";
            break;

        case LOST_MYSQL_SLAVES_CONFIG:
            msg = "Lost [mysql.slaves] config.";
            break;

        case LOST_MYSQL_SLAVES_HOST:
            msg = "Lost [mysql.slaves.host] config.";
            break;

        case LOST_MYSQL_SLAVES_DBNAME:
            msg = "Lost [mysql.slaves.dbname] config.";
            break;

        case LOST_MYSQL_SLAVES_USERNAME:
            msg = "Lost [mysql.slaves.username] config.";
            break;

        case LOST_MYSQL_SLAVES_PASSWORD:
            msg = "Lost [mysql.slaves.password] config.";
            break;

        default:
            msg = "unreachable";
    }

    return msg;
}

