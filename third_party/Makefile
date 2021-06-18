include ../Inc.mk

install:
	bash make.sh install_open_ssl openssl-1.0.2k $(LIB_DIR)
	bash make.sh install_curl curl-7.29.0 $(LIB_DIR)
	bash make.sh install_picohttpparser picohttpparser $(LIB_DIR)
	bash make.sh install_tinyxml2 tinyxml2 $(LIB_DIR)
	bash make.sh install_fmt fmt-7.1.3 $(LIB_DIR)

uninstall:
	bash make.sh uninstall_open_ssl openssl-1.0.2k $(LIB_DIR)
	bash make.sh uninstall_curl curl-7.29.0 $(LIB_DIR)
	bash make.sh uninstall_picohttpparser picohttpparser $(LIB_DIR)
	bash make.sh uninstall_tinyxml2 tinyxml2 $(LIB_DIR)
	bash make.sh uninstall_fmt fmt-7.1.3 $(LIB_DIR)