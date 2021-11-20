#==============================================================================
#       BUILD:              编译类型
#------------------------------------------------------------------------------
#       BUILD_DEBUG:        开发版本
#       BUILD_RELEASE:      发行版本
#------------------------------------------------------------------------------
BUILD = BUILD_DEBUG
#BUILD = BUILD_RELEASE

MAKE = make
CC = gcc
CXX = g++ -std=c++20
AR = ar
ARFLAGS = -scurv
RANLIB = ranlib

CFLAGS 	 ?=
CXXFLAGS ?=
INCLUDE  ?=
LDFLAGS  ?=

ifeq ($(BUILD), BUILD_DEBUG)
CFLAGS   += -Wall -ggdb3 -fPIC -pipe -Wl,-z -Wl,defs -DDEBUG -Wno-class-memaccess
CXXFLAGS += -Wall -ggdb3 -fPIC -pipe -Wl,-z -Wl,defs -DDEBUG -Wno-class-memaccess
INCLUDE  +=
LDFLAGS  +=
endif
ifeq ($(BUILD), BUILD_RELEASE)
CFLAGS   += -Wall -g -fPIC -pipe -Wl,-z -Wl,defs -O3
CXXFLAGS += -Wall -g -fPIC -pipe -Wl,-z -Wl,defs -O3
INCLUDE  +=
LDFLAGS  +=
endif

PROJ_PATH = $(shell pwd | awk -F'/zcp_tool' '{print $$1}')/zcp_tool
BIN_DIR = $(PROJ_PATH)/release/bin
LIB_DIR = $(PROJ_PATH)/release/lib
OBJ_DIR = $(PROJ_PATH)/release/obj

#由于第三方库代码里面include的路径就是按照原目录结构，导致引用第三方库不能直接使用全路径
FMT_INC 			= $(PROJ_PATH)/third_party/fmt/include
PICOHTTPPARSER_INC 	= $(PROJ_PATH)/third_party/picohttpparser/include
RAPIDJSON_INC 		= $(PROJ_PATH)/third_party/rapidjson/include
SPDLOG_INC 			= $(PROJ_PATH)/third_party/spdlog/include
TINYXML2_INC 		= $(PROJ_PATH)/third_party/tinyxml2/include
YAML_INC 			= $(PROJ_PATH)/third_party/yaml/include

INCLUDE += -I$(PROJ_PATH) -I$(FMT_INC) -I$(PICOHTTPPARSER_INC) \
		   -I$(RAPIDJSON_INC) -I$(SPDLOG_INC) -I$(TINYXML2_INC) \
		   -I$(YAML_INC)

FORMAT_INIT = $(PROJ_PATH)/tool/clang-format --style=Google --dump-config > .clang-format
FORMAT = $(PROJ_PATH)/tool/clang-format --style=file --fallback-style=none -i

#自动计算文件的依赖关系
%.d: %.c
	$(CC) $(INCLUDE) -MD $< > $@
	@$(CC) $(INCLUDE) -MD $< | sed s/"^"/"\."/ | sed s/"^\. "/" "/ | \
                sed s/"\.o"/"\.d"/  >> $@
%.d: %.cc
	$(CXX) $(INCLUDE) -MD $< > $@
	@$(CXX) $(INCLUDE) -MD $< | sed s/"^"/"\."/ | sed s/"^\. "/" "/ | \
                sed s/"\.o"/"\.d"/  >> $@
%.d: %.cpp
	$(CXX) $(INCLUDE) -MD $< > $@
	@$(CXX) $(INCLUDE) -MD $< | sed s/"^"/"\."/ | sed s/"^\. "/" "/ | \
                sed s/"\.o"/"\.d"/  >> $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
%.o: %.cc
	$(CXX) $(CFLAGS) $(INCLUDE) -c $< -o $@
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@
%.o: %.S
	$(CXX) $(CXXFLAGS) -D__WITH_FLOAT_SUPPORT -c $^ -o $@

$(OBJ_DIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $(OBJ_DIR)$*.o
$(OBJ_DIR)%.o: %.cc
	$(CXX) $(CFLAGS) $(INCLUDE) -c $< -o $(OBJ_DIR)$*.o
$(OBJ_DIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $(OBJ_DIR)$*.o
$(OBJ_DIR)%.o: %.S
	$(CXX) $(CXXFLAGS) -D__WITH_FLOAT_SUPPORT -c $^ -o $(OBJ_DIR)$*.o