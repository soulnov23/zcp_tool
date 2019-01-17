#!/bin/bash

set -x

function make_open_ssl()
{
	cd $1
	./config shared --prefix=/usr/local/ssl
	make
	make install
	cp -f /usr/local/ssl/lib/libcrypto.* /usr/local/ssl/lib/libssl.* $2
}

function clean_open_ssl()
{
	cd $1
	make clean
}

function make_curl()
{
	cd $1
	./configure --prefix=/usr/local/curl --with-ssl=/usr/local/ssl
	make
	make install
	cp -f /usr/local/curl/lib/libcurl.* $2
}

function clean_curl()
{
	cd $1
	make clean
}

function make_scons()
{
	cd $1
	python setup.py install
}

function clean_scons()
{
	cd $1
	rm -rf build
}

function make_json()
{
	cd $1
	scons platform=linux-gcc
	cp -f libs/linux-gcc*/libjson_linux-gcc*libmt.a $2/libjson.a
	cp -f libs/linux-gcc*/libjson_linux-gcc*libmt.so $2/libjson.so
}

function clean_json()
{
	cd $1
	rm -rf libs
	rm -rf bin
	rm -rf buildscons
	rm -rf dist
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
	make_scons)
		make_scons $2
		;;
	clean_scons)
		clean_scons $2
		;;
	make_json)
		make_json $2 $3
		;;
	clean_json)
		clean_json $2
		;;
	*)
		echo "error:argument is invalid"
		;;
	esac
}

main "$@"
