#!/bin/bash -l
/etc/init.d/redis-server start
/etc/init.d/rabbitmq-server start
#rabbitmqctl add_user agile agile;
#rabbitmqctl set_user_tags agile administrator;
#rabbitmqctl set_permissions -p / agile ".*" ".*" ".*";
rabbitmqadmin declare exchange name=ttn.handler type=topic auto_delete=false durable=true;

CMD=$1
shift
$CMD $@
