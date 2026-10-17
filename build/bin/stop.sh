#!/bin/bash

curtime=`date "+%Y-%m-%d %H:%M:%S.%6N"`
echo "[$curtime] [echo] stop..."

#./zcp_tool -s stop 1>>../log/frame.log 2>&1

killall -9 zcp_tool

ps -ef | grep zcp_tool | grep -v grep | grep -v vi | grep -v tail | grep -v kill