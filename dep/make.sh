#!/bin/bash

:<<!
LOG_FILE="/var/log/make.log"
>"${LOG_FILE}"
exec &>>${LOG_FILE}
set -x
!

set -x

function install_open_ssl()
{
	cd $1
	chmod -R 777 ./
	#Makefile is older than Makefile.org, Configure or config.
	./config
	./config shared --prefix=/tmp/ssl
	make
	make install
	cp -f /tmp/ssl/lib/libcrypto.* /tmp/ssl/lib/libssl.* $2
	make clean
	rm -rf /tmp/ssl
}

function uninstall_open_ssl()
{
	cd $1
	make clean
	rm -rf /tmp/ssl
	rm -rf $2/libcrypto.* $2/libssl.*
}

function install_curl()
{
	cd $1
	chmod -R 777 ./
	./configure --prefix=/tmp/curl --without-nss --with-ssl=/tmp/ssl
	make
	make install
	cp -f /tmp/curl/lib/libcurl.* $2
	make clean
	rm -rf ./docs/examples/.deps
	rm -rf ./lib/.deps
	rm -rf ./src/.deps
	rm -rf ./tests/libtest/.deps
	rm -rf ./tests/server/.deps
	rm -rf ./tests/unit/.deps
	rm -rf /tmp/curl
}

function uninstall_curl()
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
	rm -rf $2/libcurl.*
}

function install_libco()
{
	yum -y install libaio libaio-devel
	cd $1
	make
	cp ./lib/* $2
	cp ./solib/* $2
	make clean
}

function uninstall_libco()
{
	cd $1
	make clean
	rm -rf $2/libcolib.*
}

function install_gperf()
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
	make clean
	rm -rf /tmp/gperf
}

function uninstall_gperf()
{
	cd $1
	make clean
	rm -rf /tmp/gperf
	rm -rf $2/libtcmalloc*.*
}

main()
{
	case $1 in
	install_open_ssl)
		install_open_ssl $2 $3
		;;
	uninstall_open_ssl)
		uninstall_open_ssl $2 $3
		;;
	install_curl)
		install_curl $2 $3
		;;
	uninstall_curl)
		uninstall_curl $2 $3
		;;
	install_libco)
		install_libco $2 $3
		;;
	uninstall_libco)
		uninstall_libco $2 $3
		;;
	install_gperf)
		install_gperf $2 $3
		;;
	uninstall_gperf)
		uninstall_gperf $2 $3
		;;
	*)
		echo "error:argument is invalid"
		;;
	esac
}

main "$@"