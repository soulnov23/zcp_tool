#==============================================================================
#       BUILD:              编译类型
#------------------------------------------------------------------------------
#       BUILD_DEBUG:        开发版本
#       BUILD_RELEASE:      发行版本
#------------------------------------------------------------------------------
BUILD = BUILD_DEBUG
#BUILD = BUILD_RELEASE

CC = gcc
CXX = g++ -std=c++2a
AR = ar
ARFLAGS = -scurv
RANLIB = ranlib

CFLAGS ?=
CXXFLAGS ?=
INCLUDE ?=
LDFLAGS ?=

ifeq ($(BUILD), BUILD_DEBUG)
CFLAGS   += -Wall -ggdb3 -fPIC -pipe -Wl,-z -Wl,defs -DDEBUG
CXXFLAGS += -Wall -ggdb3 -fPIC -pipe -Wl,-z -Wl,defs -DDEBUG
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
BIN_DIR = $(PROJ_PATH)/bin
LIB_DIR = $(PROJ_PATH)/lib
OBJ_DIR = $(PROJ_PATH)/obj

INCLUDE = -I/data/home -I/data/home/zcp_tool

COMM_PATH = $(PROJ_PATH)/comm
COMM_INC = $(COMM_PATH)

OPEN_SSL_PATH = $(PROJ_PATH)/dep/openssl-1.0.2k
OPEN_SSL_INC = $(OPEN_SSL_PATH)/include

CURL_PATH = $(PROJ_PATH)/dep/curl-7.29.0
CURL_INC = $(CURL_PATH)/include

RAPID_JSON_PATH = $(PROJ_PATH)/dep/rapidjson
RAPID_JSON_INC = $(RAPID_JSON_PATH)

XML_PATH = $(PROJ_PATH)/dep/tinyxml2
XML_INC = $(XML_PATH)

HTTP_PARSER_PATH = $(PROJ_PATH)/dep/http-parser
HTTP_PARSER_INC = $(HTTP_PARSER_PATH)

LIBCO_PATH = $(PROJ_PATH)/dep/libco
LIBCO_INC = $(LIBCO_PATH)

GPERF_PATH = $(PROJ_PATH)/dep/gperftools
GPERF_INC = $(GPERF_PATH)/src
GPERF_FLAGS ?= -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free

FORMAT_INIT = $(PROJ_PATH)/tool/clang-format --style=Google --dump-config > .clang-format
FORMAT = $(PROJ_PATH)/tool/clang-format --style=file --fallback-style=none -i

#自动计算文件的依赖关系
.%.d: %.cpp
	$(CC) $(INCLUDE) -MM $< > $@
	@$(CC) $(INCLUDE) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@
.%.d: %.cc
	$(CC) $(INCLUDE) -MM $< > $@
	@$(CC) $(INCLUDE) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<
%.o: %.cc 
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

$(OBJ_DIR)%.o: %.cpp
	$(CXX) -o $(OBJ_DIR)$*.o $(CXXFLAGS) $(INCLUDE) -c $<
$(OBJ_DIR)%.o: %.cc
	$(CXX) -o $(OBJ_DIR)$*.o $(CXXFLAGS) $(INCLUDE) -c $<

.%.d: %.c
	$(CC) $(INCLUDE) -MM $< > $@
	@$(CC) $(INCLUDE) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

$(OBJ_DIR)%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

$(OBJ_DIR)%.o: %.S
	$(CXX) $(CXXFLAGS) -D__WITH_FLOAT_SUPPORT -o $@ -c $^