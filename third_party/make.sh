#!/bin/bash

:<<!
LOG_FILE="/var/log/make.log"
>"${LOG_FILE}"
exec &>>${LOG_FILE}
set -x
!

set -x

function openssl()
{
	tar -zvxf openssl-1.1.1j.tar.gz
	cd openssl-1.1.1j
	chmod -R 755 ./
	#Makefile is older than Makefile.org, Configure or config.
	./config
	./config shared --prefix=/usr
	make -j32
	make install
	rm -rf openssl-1.1.1j
}

function curl()
{
	tar -zvxf curl-7.76.1.tar.gz
	cd curl-7.76.1
	chmod -R 755 ./
	./configure --prefix=/usr --with-ssl
	make -j32
	make install
	rm -rf curl-7.76.1
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