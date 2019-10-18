#获取当前正在执行的makefile的绝对路径，gnu make会自动将所有读取的makefile路径都会加入到MAKEFILE_LIST变量
make_file_path := $(abspath $(lastword $(MAKEFILE_LIST)))
proj_path := $(shell echo $(dir $(make_file_path)) | sed 's/.$$//')
export PROJ_PATH=$(proj_path)
export BIN_DIR=$(PROJ_PATH)/bin
export LIB_DIR=$(PROJ_PATH)/lib
export OBJ_DIR=$(PROJ_PATH)/obj

DIR_DEP=dep

DIR_MODULES=comm    \
            example \
            lib     \
            obj     \
            src

all:
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir; \
		echo; \
	done

clean:
	@for dir in $(DIR_DEP); \
	do \
		make -C $$dir clean; \
		echo; \
	done
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir clean; \
		echo; \
	done

install:
	@for dir in $(DIR_DEP); \
	do \
		make -C $$dir; \
		echo; \
	done
	for dir in $(DIR_DEP); \
	do \
		make -C $$dir clean; \
		echo; \
	done