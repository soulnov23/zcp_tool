#!/bin/bash

curtime=`date "+%Y-%m-%d %H:%M:%S.%6N"`
echo "[$curtime] [echo] init..."

#LIB_PATH需要修改为对应的路径
LIB_PATH=../lib
export LD_LIBRARY_PATH=$LIB_PATH:$LD_LIBRARY_PATH
ulimit -c unlimited
mkdir -p ../log