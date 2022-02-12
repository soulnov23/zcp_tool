#!/bin/bash

./stop.sh
curtime=`date "+%Y-%m-%d %H:%M:%S.%6N"`
echo "[$curtime] [echo] sleep 2s..."
sleep 2
./start.sh