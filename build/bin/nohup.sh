#!/bin/bash

source init.sh

killall -SIGINT zcp_tool
nohup ./zcp_tool -c ../conf/server.yaml >/dev/null 2>&1 &
ps -ef | grep zcp_tool | grep -v grep | grep -v vi | grep -v tail | grep -v kill
