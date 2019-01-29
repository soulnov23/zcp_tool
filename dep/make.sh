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
	./configure --prefix=/tmp/curl --without-nss --with-ssl=/tmp/ssl
	make
	make install
	cp -f /tmp/curl/bin/curl /usr/bin/
	cp -f /tmp/curl/bin/curl-config /usr/bin/
	cp -f /tmp/curl/lib/libcurl.* $2
}

function clean_curl()
{
	cd $1
	make clean
	rm -rf /tmp/curl
}

function make_libco()
{
	cd $1
	make
	cp ./lib/* $2
	cp ./solib/* $2
}

function clean_libco()
{
	cd $1
	make clean
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
	make_libco)
		make_libco $2 $3
		;;
	clean_libco)
		clean_libco $2
		;;
	*)
		echo "error:argument is invalid"
		;;
	esac
}

main "$@"