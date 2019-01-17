include ../Inc.mk

all:
	bash make.sh make_open_ssl $(OPEN_SSL_PATH) $(LIB_DIR)
	bash make.sh make_curl $(CURL_PATH) $(LIB_DIR)
	bash make.sh make_scons $(SCONS_PATH)
	bash make.sh make_json $(JSON_PATH) $(LIB_DIR)
	make -C $(XML_PATH)

clean:
	bash make.sh clean_open_ssl $(OPEN_SSL_PATH)
	bash make.sh clean_curl $(CURL_PATH)
	bash make.sh clean_scons $(SCONS_PATH)
	bash make.sh clean_json $(JSON_PATH)
	make -C $(XML_PATH) clean

