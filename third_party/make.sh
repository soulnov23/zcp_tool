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
	cd $1
	chmod -R 755 ./
	#Makefile is older than Makefile.org, Configure or config.
	./config
	./config shared --prefix=/tmp/ssl
	make -j32
	cp -f /tmp/ssl/lib/libcrypto.* /tmp/ssl/lib/libssl.* $2
}

function clean_openssl()
{
	cd $1
	make clean
	rm -rf /tmp/ssl
	rm -f $2/libcrypto.* $2/libssl.*
}

function curl()
{
	cd $1
	chmod -R 755 ./
	./configure --prefix=/tmp/curl --without-nss --with-ssl=/tmp/ssl
	make -j32
	cp -f /tmp/curl/lib/libcurl.* $2
}

function clean_curl()
{
	cd $1
	make clean
	rm -rf /tmp/curl
	rm -f $2/libcurl.*
}

function picohttpparser()
{
	cd $1
	make -j32
}

function clean_picohttpparser()
{
	cd $1
	make clean
}

function tinyxml2()
{
	cd $1
	make -j32
}

function clean_tinyxml2()
{
	cd $1
	make clean
}

function fmt()
{
	cd $1
	make -j32
}

function clean_fmt()
{
	cd $1
	make clean
}

main()
{
	case $1 in
	openssl)
		openssl $2 $3
		;;
	clean_openssl)
		clean_openssl $2 $3
		;;
	curl)
		curl $2 $3
		;;
	clean_curl)
		clean_curl $2 $3
		;;
	picohttpparser)
		picohttpparser $2 $3
		;;
	clean_picohttpparser)
		clean_picohttpparser $2 $3
		;;
	tinyxml2)
		tinyxml2 $2 $3
		;;
	clean_tinyxml2)
		clean_tinyxml2 $2 $3
		;;
	fmt)
		fmt $2 $3
		;;
	clean_fmt)
		clean_fmt $2 $3
		;;
	*)
		echo "error:argument is invalid"
		;;
	esac
}

main "$@"