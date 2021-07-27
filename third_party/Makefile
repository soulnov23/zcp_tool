include ../Inc.mk

OPENSSL := openssl-1.1.1j
CURL := curl-7.76.1

#all: openssl curl fmt picohttpparser tinyxml2 yaml
all: fmt picohttpparser tinyxml2 yaml

openssl: 
	tar -zvxf $(OPENSSL).tar.gz
	chmod -R 755 $(OPENSSL)
	#Makefile is older than Makefile.org, Configure or config.
	cd $(OPENSSL) && ./config && ./config shared --prefix=/tmp/openssl
	cd $(OPENSSL) && $(MAKE) -j32 && $(MAKE) install
	rm -rf $(OPENSSL)
	rm -rf openssl
	cp -rf /tmp/openssl/include ./openssl
	cp -rf /tmp/openssl/lib/lib* $(LIB_DIR)

curl:
	tar -zvxf $(CURL).tar.gz
	chmod -R 755 $(CURL)
	cd $(CURL) && ./configure --prefix=/tmp/curl --with-ssl=/tmp/openssl
	cd $(CURL) && $(MAKE) -j32 && $(MAKE) install
	rm -rf $(CURL)
	rm -rf curl
	cp -rf /tmp/curl/include ./curl
	cp -rf /tmp/curl/lib/lib* $(LIB_DIR)

fmt:
	cd fmt && \
	rm -rf build && \
	mkdir build && \
	cd build && \
	cmake3 -DBUILD_SHARED_LIBS=TRUE -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE ../ && \
	$(MAKE) -j32
	cp -rf fmt/build/libfmt.so* $(LIB_DIR)
	rm -rf fmt/build

picohttpparser:
	$(MAKE) -C picohttpparser -j32

tinyxml2:
	$(MAKE) -C tinyxml2 -j32

clean:
	rm -rf $(LIB_DIR)/libcrypto.*
	rm -rf $(LIB_DIR)/libssl.*
	rm -rf /tmp/openssl
	rm -rf $(LIB_DIR)/libcurl.*
	rm -rf /tmp/curl
	rm -rf fmt/build
	$(MAKE) -C picohttpparser clean
	$(MAKE) -C tinyxml2 clean

.PHONY: openssl curl fmt picohttpparser tinyxml2 yaml clean

.DEFAULT_GOAL: all