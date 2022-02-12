#!/bin/bash

source init.sh

curtime=`date "+%Y-%m-%d %H:%M:%S.%6N"`
echo "[$curtime] [echo] start..."

./zcp_tool -c ../conf/server.yaml
procs=`ps -ef | grep zcp_tool | grep -v grep | grep -v vi | grep -v tail | grep -v kill -c`
if [ $procs -lt 1 ];then
	curtime=`date "+%Y-%m-%d %H:%M:%S.%6N"`
	echo "[$curtime] [echo] start zcp_tool failed!"
	exit 0
else
	curtime=`date "+%Y-%m-%d %H:%M:%S.%6N"`
	echo "[$curtime] [echo] start zcp_tool success"
	exit 0
fi