#!/bin/bash

source init.sh

killall -SIGINT go-tool
./zcp_tool -c ../conf/server.yaml
