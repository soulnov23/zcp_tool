include Inc.mk
include ../Inc.mk

all: fmt googletest jsoncpp picohttpparser rapidjson spdlog tinyxml2 yaml

fmt:
	tar -zvxf zip/${FMT}.tar.gz
	rm -rf fmt
	mv -f ${FMT} fmt
	cd fmt && \
	rm -rf build && \
	mkdir build && \
	cd build && \
	cmake3 -DBUILD_SHARED_LIBS=TRUE -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../ && \
	$(MAKE) -j32
	cp -rf fmt/build/libfmt.so* $(LIB_DIR)
	rm -rf fmt/build

googletest:
	tar -zvxf zip/${GTEST}.tar.gz
	rm -rf googletest
	mv -f ${GTEST} googletest
	cd googletest && \
	rm -rf build && \
	mkdir build && \
	cd build && \
	cmake3 -DBUILD_SHARED_LIBS=TRUE -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../ && \
	$(MAKE) -j32
	cp -rf googletest/build/lib/libgmock*.so* $(LIB_DIR)
	cp -rf googletest/build/lib/libgtest*.so* $(LIB_DIR)
	rm -rf googletest/build

jsoncpp:
	tar -zvxf zip/${JSONCPP}.tar.gz
	rm -rf jsoncpp
	mv -f ${JSONCPP} jsoncpp
	cd jsoncpp && \
	rm -rf build && \
	mkdir build && \
	cd build && \
	cmake3 -DBUILD_SHARED_LIBS=TRUE -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../ && \
	$(MAKE) -j32
	cp -rf jsoncpp/build/lib/libjsoncpp.* $(LIB_DIR)
	rm -rf jsoncpp/build

picohttpparser:
	$(MAKE) -C picohttpparser -j32

rapidjson:
	tar -zvxf zip/${RAPIDJSON}.tar.gz
	rm -rf rapidjson
	mv -f ${RAPIDJSON} rapidjson

spdlog:
	tar -zvxf zip/${SPDLOG}.tar.gz
	rm -rf spdlog
	mv -f ${SPDLOG} spdlog
	cd spdlog && \
	rm -rf build && \
	mkdir build && \
	cd build && \
	cmake3 -DBUILD_SHARED_LIBS=TRUE -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../ && \
	$(MAKE) -j32
	cp -rf spdlog/build/libspdlog.so* $(LIB_DIR)
	rm -rf spdlog/build

tinyxml2:
	$(MAKE) -C tinyxml2 -j32

yaml:
	$(MAKE) -C yaml

clean:
	rm -rf $(LIB_DIR)/libfmt.*
	rm -rf $(LIB_DIR)/libgmock*.*
	rm -rf $(LIB_DIR)/libgtest*.*
	$(MAKE) -C picohttpparser clean
	$(MAKE) -C tinyxml2 clean
	$(MAKE) -C yaml clean

.PHONY: fmt googletest jsoncpp picohttpparser rapidjson spdlog tinyxml2 yaml clean

.DEFAULT_GOAL: all