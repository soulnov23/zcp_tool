include ../Inc.mk

all:
	bash make.sh make_open_ssl $(OPEN_SSL_PATH) $(LIB_DIR)
	bash make.sh make_curl $(CURL_PATH) $(LIB_DIR)
	bash make.sh make_libco $(LIBCO_PATH) $(LIB_DIR)
	make -C $(JSON_CPP_PATH)
	make -C $(XML_PATH)

clean:
	bash make.sh clean_open_ssl $(OPEN_SSL_PATH)
	bash make.sh clean_curl $(CURL_PATH)
	bash make.sh clean_libco $(LIBCO_PATH)
	make -C $(JSON_CPP_PATH) clean
	make -C $(XML_PATH) clean

