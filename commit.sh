#!/bin/bash

:<<!
LOG_FILE="/var/log/commit.log"
>"${LOG_FILE}"
exec &>>${LOG_FILE}
set -x
!

set -x

main()
{
	make clean
	git pull
	git add .
	local note=$1
	if [ -z "$note" ];then
		git commit -a -m "null"
	else
		git commit -a -m "${note}"
	fi
	git push -u origin master
}

main "$@"
