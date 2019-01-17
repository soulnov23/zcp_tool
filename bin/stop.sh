#!/bin/bash

echo "main end..."

killall -9 main

#pid=`ps -ef | grep main | grep -v grep | grep -v vi | grep -v tail | awk '{if( match($8, "main")) printf $2"\n"}' | head -1`
#kill -s USR2 $(pid)
