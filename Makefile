include Inc.mk

all: check
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

format:
	$(MAKE) -C src format

install: check
	$(MAKE) -C third_party

uninstall:
	$(MAKE) -C third_party clean

check:
	mkdir -p $(BIN_DIR)
	mkdir -p $(LIB_DIR)

.PHONY: all clean format install uninstall check

.DEFAULT_GOAL := all