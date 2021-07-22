include Inc.mk
#获取当前正在执行的makefile的绝对路径，gnu make会自动将所有读取的makefile路径都会加入到MAKEFILE_LIST变量
#make_file_path := $(abspath $(lastword $(MAKEFILE_LIST)))
#proj_path := $(shell echo $(dir $(make_file_path)) | sed 's/.$$//')
#PROJ_PATH := $(proj_path)
#export PROJ_PATH

all: check
	$(MAKE) -C zcp_tool

clean:
	$(MAKE) -C zcp_tool clean

format:
	$(MAKE) -C zcp_tool format

install: check
	$(MAKE) -C third_party

uninstall:
	$(MAKE) -C third_party clean

check:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)