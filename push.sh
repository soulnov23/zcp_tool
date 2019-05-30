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
	rm -rf ./.vscode/ipch
	cp -rf ./lib /tmp
	make clean
	git pull
	git add -A
	local note=$1
	if [ -z "$note" ];then
		git commit -a -m "null"
	else
		git commit -a -m "${note}"
	fi
	git push -u origin master
	cp -rf /tmp/lib/* ./lib
	rm -rf /tmp/lib
}

main "$@"
