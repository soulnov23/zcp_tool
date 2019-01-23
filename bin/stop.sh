#!/bin/bash

echo "zcp_server end..."

killall -9 zcp_server

#pid=`ps -ef | grep zcp_server | grep -v grep | grep -v vi | grep -v tail | awk '{if( match($8, "zcp_server")) printf $2"\n"}' | head -1`
#kill -s USR2 $(pid)
