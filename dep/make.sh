#!/bin/bash

:<<!
LOG_FILE="/var/log/make.log"
>"${LOG_FILE}"
exec &>>${LOG_FILE}
set -x
!

set -x

function make_open_ssl()
{
	cd $1
	chmod -R 777 ./
	#Makefile is older than Makefile.org, Configure or config.
	./config
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
	chmod -R 777 ./
	./configure --prefix=/tmp/curl --without-nss --with-ssl=/tmp/ssl
	make
	make install
	cp -f /tmp/curl/lib/libcurl.* $2
}

function clean_curl()
{
	cd $1
	make clean
	rm -rf ./docs/examples/.deps
	rm -rf ./lib/.deps
	rm -rf ./src/.deps
	rm -rf ./tests/libtest/.deps
	rm -rf ./tests/server/.deps
	rm -rf ./tests/unit/.deps
	rm -rf /tmp/curl
}

function make_libco()
{
	yum -y install libaio libaio-devel
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

function make_gperf()
{
	yum -y install autoconf automake gnome-common
	cd $1
	chmod -R 777 ./
	./autogen.sh
	./configure --prefix=/tmp/gperf --disable-cpu-profiler \
	--disable-heap-profiler --disable-heap-checker \
	--disable-debugalloc --enable-minimal
	make
	make install
	cp -f /tmp/gperf/lib/libtcmalloc*.* $2/
}

function clean_gperf()
{
	cd $1
	make clean
	rm -rf /tmp/gperf
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
	make_gperf)
		make_gperf $2 $3
		;;
	clean_gperf)
		clean_gperf $2
		;;
	*)
		echo "error:argument is invalid"
		;;
	esac
}

main "$@"