#!/bin/bash

source init.sh

curtime=`date "+%Y-%m-%d %H:%M:%S"`
echo "[$curtime] [echo] start..."

procs=`ps -ef | grep zcp_tool | grep -v grep | grep -v vi | grep -v tail | grep -v kill -c`
if [ $procs -ge 1 ];then
	echo "[$curtime] [echo] zcp_tool already runing!"
	exit 0
else
	./zcp_tool -c ../conf/server.yaml 1>>../log/zcp_tool.log 2>&1
	procs=`ps -ef | grep zcp_tool | grep -v grep | grep -v vi | grep -v tail | grep -v kill -c`
	if [ $procs -lt 1 ];then
		echo "[$curtime] [echo] start zcp_tool failed!"
		exit 0
	else
		echo "[$curtime] [echo] start zcp_tool success"
		exit 0
	fi
fi
