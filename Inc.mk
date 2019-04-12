#==============================================================================
#       BUILD:              编译类型
#------------------------------------------------------------------------------
#       BUILD_DEV:          开发版本
#       BUILD_DEBUG:        测试版本
#       BUILD_NORMAL:       一般开发版本
#       BUILD_RELEASE:      发行版本
#------------------------------------------------------------------------------
BUILD=BUILD_DEBUG
#BUILD=BUILD_DEV
#BUILD=BUILD_NORMAL
#BUILD=BUILD_RELEASE

CC = gcc
CXX = g++ -std=c++11
AR ?= ar
ARFLAGS = -scurv
RANLIB ?= ranlib

CFLAGS ?=
CXXFLAGS ?=
INCLUDE ?=
LDFLAGS ?=

ifeq ($(BUILD), BUILD_DEBUG)
CFLAGS   += -Wall -g -fPIC -pipe -Wl,-z -Wl,defs -DDEBUG
CXXFLAGS += -Wall -g -fPIC -pipe -Wl,-z -Wl,defs -DDEBUG
INCLUDE  +=
LDFLAGS  +=
endif
ifeq ($(BUILD), BUILD_DEV)
CFLAGS   += -Wall -g -fPIC -pipe -Wl,-z -Wl,defs -Werror
CXXFLAGS += -Wall -g -fPIC -pipe -Wl,-z -Wl,defs -Werror
INCLUDE  +=
LDFLAGS  +=
endif
ifeq ($(BUILD), BUILD_NORMAL)
CFLAGS   += -Wall -g -fPIC -pipe -Wl,-z -Wl,defs -O
CXXFLAGS += -Wall -g -fPIC -pipe -Wl,-z -Wl,defs -O
INCLUDE  +=
LDFLAGS  +=
endif
ifeq ($(BUILD), BUILD_RELEASE)
CFLAGS   += -Wall -g -fPIC -pipe -Wl,-z -Wl,defs -O3
CXXFLAGS += -Wall -g -fPIC -pipe -Wl,-z -Wl,defs -O3
INCLUDE  +=
LDFLAGS  +=
endif

PROJ_PATH=/home/zcp_tool
BIN_DIR=$(PROJ_PATH)/bin
LIB_DIR=$(PROJ_PATH)/lib
OBJ_DIR=$(PROJ_PATH)/obj

COMM_PATH=$(PROJ_PATH)/comm
COMM_INC=$(PROJ_PATH)/comm

COMM_CRYPTO_PATH=$(PROJ_PATH)/comm/crypto
COMM_CRYPTO_INC=$(PROJ_PATH)/comm/crypto

COMM_NET_PATH=$(PROJ_PATH)/comm/net
COMM_NET_INC=$(PROJ_PATH)/comm/net

OPEN_SSL_PATH=$(PROJ_PATH)/dep/openssl-1.0.2k
OPEN_SSL_INC=$(OPEN_SSL_PATH)/include

CURL_PATH=$(PROJ_PATH)/dep/curl-7.29.0
CURL_INC=$(CURL_PATH)/include

JSON_CPP_PATH=$(PROJ_PATH)/dep/jsoncpp-src-0.5.0
JSON_CPP_INC=$(JSON_CPP_PATH)/include

RAPID_JSON_PATH=$(PROJ_PATH)/dep/rapidjson
RAPID_JSON_INC=$(PROJ_PATH)/dep/rapidjson

XML_PATH=$(PROJ_PATH)/dep/tinyxml2
XML_INC=$(PROJ_PATH)/dep/tinyxml2

LIBCO_PATH=$(PROJ_PATH)/dep/libco
LIBCO_INC=$(PROJ_PATH)/dep/libco

GPERF_PATH=$(PROJ_PATH)/dep/gperftools
GPERF_INC=$(GPERF_PATH)/src

#自动计算文件的依赖关系
.%.d: %.cpp
	$(CC) $(INCLUDE) -MM $< > $@
	@$(CC) $(INCLUDE) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $<

$(OBJ_DIR)%.o:%.cpp
	$(CXX) -o $(OBJ_DIR)$*.o $(CXXFLAGS) $(INCLUDE) -c $<

.%.d: %.c
	$(CC) $(INCLUDE) -MM $< > $@
	@$(CC) $(INCLUDE) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

$(OBJ_DIR)%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

$(OBJ_DIR)%.o : %.S
	$(CXX) $(CXXFLAGS) -D__WITH_FLOAT_SUPPORT -o $@ -c $^