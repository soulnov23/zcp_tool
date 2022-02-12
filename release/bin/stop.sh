#!/bin/bash

curtime=`date "+%Y-%m-%d %H:%M:%S.%6N"`
echo "[$curtime] [echo] stop..."

killall -9 zcp_tool

curtime=`date "+%Y-%m-%d %H:%M:%S.%6N"`
echo "[$curtime] [echo] stop zcp_tool success"

#pid=`ps -ef | grep zcp_tool | grep -v grep | grep -v vi | grep -v tail | awk '{if( match($8, "zcp_tool")) printf $2"\n"}' | head -1`
#kill -s USR2 $(pid)
