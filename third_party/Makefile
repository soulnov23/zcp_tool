include ../Inc.mk

DIR_MODULES = openssl-1.0.2k \
              curl-7.29.0 \
              gperftools \
			  libco \
			  http-parser \
			  tinyxml2

install:
	bash make.sh install_open_ssl openssl-1.0.2k $(LIB_DIR)
	bash make.sh install_curl curl-7.29.0 $(LIB_DIR)
	bash make.sh install_gperf gperftools $(LIB_DIR)
	bash make.sh install_libco libco $(LIB_DIR)
	bash make.sh install_http_parser http-parser $(LIB_DIR)
	bash make.sh install_tinyxml2 tinyxml2 $(LIB_DIR)

uninstall:
	bash make.sh uninstall_open_ssl openssl-1.0.2k $(LIB_DIR)
	bash make.sh uninstall_curl curl-7.29.0 $(LIB_DIR)
	bash make.sh uninstall_gperf gperftools $(LIB_DIR)
	bash make.sh uninstall_libco libco $(LIB_DIR)
	bash make.sh uninstall_http_parser http-parser $(LIB_DIR)
	bash make.sh uninstall_tinyxml2 tinyxml2 $(LIB_DIR)