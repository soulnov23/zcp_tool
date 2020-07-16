#!/bin/bash

curtime=`date "+%Y-%m-%d %H:%M:%S"`
echo "[$curtime] [echo] restart..."

./stop.sh
echo "[$curtime] [echo] sleep 5s..."
sleep 5
./start.sh

echo "[$curtime] [echo] restart zcp_server success"