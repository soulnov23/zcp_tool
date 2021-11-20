#!/bin/bash

./stop.sh
curtime=`date "+%Y-%m-%d %H:%M:%S"`
echo "[$curtime] [echo] sleep 5s..."
sleep 5
./start.sh