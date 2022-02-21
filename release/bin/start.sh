#!/bin/bash

curtime=`date "+%Y-%m-%d %H:%M:%S.%6N"`
echo "[$curtime] [echo] start..."

source init.sh

./zcp_tool -c ../conf/server.yaml 1>>../log/frame.log 2>&1

ps -ef | grep zcp_tool | grep -v grep | grep -v vi | grep -v tail | grep -v kill