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
	git commit -m "${note}" 
	git push -u origin master
}

main "$@"
