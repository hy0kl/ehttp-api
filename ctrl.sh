#!/bin/bash
# @describe:
# @author:   Jerry Yang(hy0kle@gmail.com)

#set -x

server_conf="conf/server.json"
if [[ ! -f "$server_conf" ]]
then
    echo "Can NOT find $server_conf"
    echo "Please see README.md and fix it."
    exit -1
fi

bin_name="api-server"
server_port=`grep port $server_conf | head -n 1 | awk '{split($2, cntr, ","); print cntr[1]}'`
prog="$bin_name:$server_port"
#echo $prog

work_pid=""
function get_work_pid()
{
    work_pid=$(ps aux | grep $prog | grep -v grep | awk '{print $2}' | xargs)
}

get_work_pid

case "$1" in
    start)
        if ((work_pid > 0))
        then
            echo "$prog is working... pid: $work_pid"
            exit 1
        else
            cmd="./$bin_name"
            nohup $cmd &
            echo "$prog start work."
        fi
    ;;

    stop)
        if ((work_pid > 0))
        then
            kill -9 "$work_pid"
            echo "stop $prog success, pid: $work_pid"
        else
            echo "Can find $prog, please check it out."
        fi
    ;;

    restart)
        $0 stop
        $0 start
    ;;

    status)
        if ((work_pid > 0))
        then
            echo "$prog is working, pid: $work_pid"
        else
            echo "$prog is not working..."
        fi
    ;;

    *)
        echo "Usage: $0 {start|stop|restart|status}"
        exit 11
    ;;
esac

exit 0

# vim:set ts=4 sw=4 et fdm=marker:

