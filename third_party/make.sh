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
	chmod -R 755 ./
	#Makefile is older than Makefile.org, Configure or config.
	./config
	./config shared --prefix=/tmp/ssl
	make -j32
	cp -f /tmp/ssl/lib/libcrypto.* /tmp/ssl/lib/libssl.* $2
}

function uninstall_open_ssl()
{
	cd $1
	make clean
	rm -rf /tmp/ssl
	rm -f $2/libcrypto.* $2/libssl.*
}

function install_curl()
{
	cd $1
	chmod -R 755 ./
	./configure --prefix=/tmp/curl --without-nss --with-ssl=/tmp/ssl
	make -j32
	cp -f /tmp/curl/lib/libcurl.* $2
}

function uninstall_curl()
{
	cd $1
	make clean
	rm -rf /tmp/curl
	rm -f $2/libcurl.*
}

function install_libco()
{
	yum -y install libaio libaio-devel
	cd $1
	make -j32
	cp -f ./lib/* $2
	cp -f ./solib/* $2
}

function uninstall_libco()
{
	cd $1
	make clean
	rm -f $2/libcolib.*
}

function install_gperf()
{
	yum -y install autoconf automake gnome-common
	cd $1
	chmod -R 755 ./
	./autogen.sh
	./configure --prefix=/tmp/gperf --disable-cpu-profiler \
	--disable-heap-profiler --disable-heap-checker \
	--disable-debugalloc --enable-minimal
	make -j32
	cp -f /tmp/gperf/lib/libtcmalloc*.* $2/
}

function uninstall_gperf()
{
	cd $1
	make clean
	rm -rf /tmp/gperf
	rm -f $2/libtcmalloc*.*
}

function install_picohttpparser()
{
	cd $1
	make -j32
}

function uninstall_picohttpparser()
{
	cd $1
	make uninstall
}

function install_tinyxml2()
{
	cd $1
	make -j32
}

function uninstall_tinyxml2()
{
	cd $1
	make uninstall
}

function install_fmt()
{
	cd $1
	cmake3 .
	make -j32
	cp -f libfmt.a $2/
}

function uninstall_fmt()
{
	cd $1
	make clean
	rm -f $2/libfmt.a
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
	install_picohttpparser)
		install_picohttpparser $2 $3
		;;
	uninstall_picohttpparser)
		uninstall_picohttpparser $2 $3
		;;
	install_tinyxml2)
		install_tinyxml2 $2 $3
		;;
	uninstall_tinyxml2)
		uninstall_tinyxml2 $2 $3
		;;
	install_fmt)
		install_fmt $2 $3
		;;
	uninstall_fmt)
		uninstall_fmt $2 $3
		;;
	*)
		echo "error:argument is invalid"
		;;
	esac
}

main "$@"