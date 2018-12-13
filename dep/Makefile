include ../Inc.mk

all:
	bash make.sh make_curl $(CURL_PATH) $(PROJ_LIB)
	bash make.sh make_scons $(SCONS_PATH)
	bash make.sh make_json $(JSON_PATH) $(PROJ_LIB)

clean:
	bash make.sh clean_curl $(CURL_PATH)
	bash make.sh clean_scons $(SCONS_PATH)
	bash make.sh clean_json $(JSON_PATH)

