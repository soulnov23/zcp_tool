ALLDIRS= comm/comm\
     comm/oidbcomm\
     comm/oidbapi_0x710\
     server


ifeq ($(BUILD_MACHINE), CI)
	include Inc_ci.mk
else
	include Inc.mk
endif

CurrentDir := $(shell pwd)
MARKETING_PLATFORM_HOME := $(shell echo ${CurrentDir}/../)

all:
	echo ${MARKETING_PLATFORM_HOME}
	@for i in ${ALLDIRS}; \
	do \
		cd ${MARKETING_PLATFORM_HOME}/;cd ${MARKETING_PLATFORM_HOME}/src/$$i >/dev/null ; \
		echo "Makeing $$i"; \
		make; \
		cd ${MARKETING_PLATFORM_HOME}/src/ >/dev/null ; \
		echo "Done"; \
	done

clean:
	@for i in ${ALLDIRS}; \
	do \
		cd ${MARKETING_PLATFORM_HOME}/;cd ${MARKETING_PLATFORM_HOME}/src/$$i >/dev/null; \
		echo "Cleaning $$i"; \
		make clean; \
		cd ${MARKETING_PLATFORM_HOME}/src/ >/dev/null; \
		echo "Done"; \
	done
	
install:
	echo ${MARKETING_PLATFORM_HOME}
	@for i in ${ALLDIRS}; \
	do \
		cd ${MARKETING_PLATFORM_HOME}/;cd ${MARKETING_PLATFORM_HOME}/src/$$i >/dev/null ; \
		echo "Makeing $$i"; \
		make install; \
		cd ${MARKETING_PLATFORM_HOME}/src/ >/dev/null ; \
		echo "Done"; \
	done

