#!/bin/bash

:<<!
LOG_FILE="/var/log/make.log"
>"${LOG_FILE}"
exec &>>${LOG_FILE}
set -x
!

set -x

OPENSSL=openssl-1.1.1j
CURL=curl-7.76.1

function openssl()
{
	tar -zvxf ${OPENSSL}.tar.gz
	chmod -R 755 ${OPENSSL}
	cd ${OPENSSL}
	#Makefile is older than Makefile.org, Configure or config.
	./config
	./config shared --prefix=/tmp/openssl
	make -j32
	make install
	cd ..
	rm -rf ${OPENSSL}
}

function curl()
{
	tar -zvxf ${CURL}.tar.gz
	chmod -R 755 ${CURL}
	cd ${CURL}
	./configure --prefix=/tmp/curl --with-ssl=/tmp/openssl
	make -j32
	make install
	cd ..
	rm -rf ${CURL}
}

main()
{
	case $1 in
	openssl)
		openssl
		;;
	curl)
		curl
		;;
	*)
		echo "error:argument is invalid"
		;;
	esac
}

main "$@"