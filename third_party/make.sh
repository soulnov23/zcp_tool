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

function install_gtest()
{
	cd $1
	cmake3 .
	make -j32
	cp -f ./lib/libgmock*.* ./lib/libgtest*.* $2/
}

function uninstall_gtest()
{
	cd $1
	make clean
	rm -f $2/libgmock*.* $2/libgtest*.*
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
	install_gtest)
		install_gtest $2 $3
		;;
	uninstall_gtest)
		uninstall_gtest $2 $3
		;;
	*)
		echo "error:argument is invalid"
		;;
	esac
}

main "$@"