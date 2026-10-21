include Inc.mk

all:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

format:
	$(MAKE) -C src format

install:
	$(MAKE) -C third_party

uninstall:
	$(MAKE) -C third_party clean

#串行
.NOTPARALLEL:

.PHONY: all clean format install uninstall

.DEFAULT_GOAL := all
