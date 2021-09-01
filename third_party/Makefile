include ../Inc.mk

all: fmt googletest picohttpparser tinyxml2 yaml
	
fmt:
	cd fmt && \
	rm -rf build && \
	mkdir build && \
	cd build && \
	cmake3 -DBUILD_SHARED_LIBS=TRUE -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../ && \
	$(MAKE) -j32
	cp -rf fmt/build/libfmt.so* $(LIB_DIR)
	rm -rf fmt/build

googletest:
	cd googletest && \
	rm -rf build && \
	mkdir build && \
	cd build && \
	cmake3 ../ && \
	$(MAKE) -j32
	cp -rf googletest/build/lib/libgmock*.a $(LIB_DIR)
	cp -rf googletest/build/lib/libgtest*.a $(LIB_DIR)
	rm -rf googletest/build

picohttpparser:
	$(MAKE) -C picohttpparser -j32

tinyxml2:
	$(MAKE) -C tinyxml2 -j32

yaml:
	$(MAKE) -C yaml

clean:
	rm -rf $(LIB_DIR)/libfmt.*
	rm -rf $(LIB_DIR)/libgmock*.a
	rm -rf $(LIB_DIR)/libgtest*.a
	$(MAKE) -C picohttpparser clean
	$(MAKE) -C tinyxml2 clean
	$(MAKE) -C yaml clean

.PHONY: fmt googletest picohttpparser tinyxml2 yaml clean

.DEFAULT_GOAL: all