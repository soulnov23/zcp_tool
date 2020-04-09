include ../Inc.mk

install:
	bash make.sh install_open_ssl $(OPEN_SSL_PATH) $(LIB_DIR)
	bash make.sh install_curl $(CURL_PATH) $(LIB_DIR)
	bash make.sh install_libco $(LIBCO_PATH) $(LIB_DIR)
	bash make.sh install_gperf $(GPERF_PATH) $(LIB_DIR)
	make -C $(XML_PATH) install
	make -C $(HTTP_PARSER_PATH) install

uninstall:
	bash make.sh uninstall_open_ssl $(OPEN_SSL_PATH) $(LIB_DIR)
	bash make.sh uninstall_curl $(CURL_PATH) $(LIB_DIR)
	bash make.sh uninstall_libco $(LIBCO_PATH) $(LIB_DIR)
	bash make.sh uninstall_gperf $(GPERF_PATH) $(LIB_DIR)
	make -C $(XML_PATH) uninstall
	make -C $(HTTP_PARSER_PATH) uninstall