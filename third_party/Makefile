include ../Inc.mk

all:
	bash make.sh open_ssl openssl $(LIB_DIR)
	bash make.sh curl curl $(LIB_DIR)
	bash make.sh picohttpparser picohttpparser $(LIB_DIR)
	bash make.sh tinyxml2 tinyxml2 $(LIB_DIR)
	bash make.sh fmt fmt $(LIB_DIR)

clean:
	bash make.sh clean_open_ssl openssl $(LIB_DIR)
	bash make.sh clean_curl curl $(LIB_DIR)
	bash make.sh clean_picohttpparser picohttpparser $(LIB_DIR)
	bash make.sh clean_tinyxml2 tinyxml2 $(LIB_DIR)
	bash make.sh clean_fmt fmt $(LIB_DIR)