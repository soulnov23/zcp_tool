include ../Inc.mk

install: make_install make_clean

make_install:
	bash make.sh install_open_ssl $(OPEN_SSL_PATH) $(LIB_DIR)
	bash make.sh install_curl $(CURL_PATH) $(LIB_DIR)
	bash make.sh install_libco $(LIBCO_PATH) $(LIB_DIR)
	bash make.sh install_gperf $(GPERF_PATH) $(LIB_DIR)
	make -C $(JSON_CPP_PATH) install
	make -C $(XML_PATH) install
	make -C $(HTTP_PARSER_PATH) install

make_clean:
	bash make.sh clean_open_ssl $(OPEN_SSL_PATH)
	bash make.sh clean_curl $(CURL_PATH)
	bash make.sh clean_libco $(LIBCO_PATH)
	bash make.sh clean_gperf $(GPERF_PATH)
	make -C $(JSON_CPP_PATH) clean
	make -C $(XML_PATH) clean
	make -C $(HTTP_PARSER_PATH) clean

uninstall:
	bash make.sh uninstall_open_ssl $(OPEN_SSL_PATH) $(LIB_DIR)
	bash make.sh uninstall_curl $(CURL_PATH) $(LIB_DIR)
	bash make.sh uninstall_libco $(LIBCO_PATH) $(LIB_DIR)
	bash make.sh uninstall_gperf $(GPERF_PATH) $(LIB_DIR)
	make -C $(JSON_CPP_PATH) uninstall
	make -C $(XML_PATH) uninstall
	make -C $(HTTP_PARSER_PATH) uninstall