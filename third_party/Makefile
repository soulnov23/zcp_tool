include ../Inc.mk

DIR_MODULES = picohttpparser \
			  tinyxml2 \
              fmt

all:
	bash make.sh openssl
	bash make.sh curl
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir -j32; \
		echo; \
	done

clean:
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir clean; \
		echo; \
	done