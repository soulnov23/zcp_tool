#!/bin/bash

set -x

function make_open_ssl()
{
	cd $1
	./config shared --prefix=/tmp/ssl
	make
	make install
	cp -f /tmp/ssl/lib/libcrypto.* /tmp/ssl/lib/libssl.* $2
}

function clean_open_ssl()
{
	cd $1
	make clean
	rm -rf /tmp/ssl
}

function make_curl()
{
	cd $1
	./configure --prefix=/tmp/curl --with-ssl=/tmp/ssl
	make
	make install
	cp -f /tmp/curl/lib/libcurl.* $2
}

function clean_curl()
{
	cd $1
	make clean
	rm -rf /tmp/curl
}

main()
{
	case $1 in
	make_open_ssl)
		make_open_ssl $2 $3
		;;
	clean_open_ssl)
		clean_open_ssl $2
		;;
	make_curl)
		make_curl $2 $3
		;;
	clean_curl)
		clean_curl $2
		;;
	*)
		echo "error:argument is invalid"
		;;
	esac
}

main "$@"
