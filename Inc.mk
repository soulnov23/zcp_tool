#==============================================================================
#       BUILD:              编译类型
#------------------------------------------------------------------------------
#       BUILD_DEV:          开发版本
#       BUILD_DEBUG:        测试版本
#       BUILD_NORMAL:       一般开发版本
#       BUILD_RELEASE:      发行版本
#------------------------------------------------------------------------------
#BUILD=BUILD_NORMAL
BUILD=BUILD_DEBUG

CC = gcc
CXX = g++
AR ?= ar
ARFLAGS = -scurv
RANLIB ?= ranlib

CFLAGS ?=
CXXFLAGS ?=
INCLUDE ?=
LDFLAGS ?=

ifeq ($(BUILD), BUILD_DEBUG)
CFLAGS   += -Wall -g -fPIC -Wl,-z -Wl,defs -DDEBUG
CXXFLAGS += -Wall -g -fPIC -Wl,-z -Wl,defs -DDEBUG
INCLUDE  +=
LDFLAGS  +=
endif
ifeq ($(BUILD), BUILD_DEV)
CFLAGS   += -Wall -g -fPIC -Wl,-z -Wl,defs -DDEBUG -Werror
CXXFLAGS += -Wall -g -fPIC -Wl,-z -Wl,defs -DDEBUG -Werror
INCLUDE  +=
LDFLAGS  +=
endif
ifeq ($(BUILD), BUILD_NORMAL)
CFLAGS   += -Wall -g -fPIC -Wl,-z -Wl,defs -O
CXXFLAGS += -Wall -g -fPIC -Wl,-z -Wl,defs -O
INCLUDE  +=
LDFLAGS  +=
endif
ifeq ($(BUILD), BUILD_RELEASE)
CFLAGS   += -Wall -g -fPIC -Wl,-z -Wl,defs -O3
CXXFLAGS += -Wall -g -fPIC -Wl,-z -Wl,defs -O3
INCLUDE  +=
LDFLAGS  +=
endif

PROJ_PATH=/home/zcp_tool
BIN_DIR=$(PROJ_PATH)/bin
LIB_DIR=$(PROJ_PATH)/lib
OBJ_DIR=$(PROJ_PATH)/obj

COMM_PATH=$(PROJ_PATH)/comm
COMM_INC=$(PROJ_PATH)/comm

OPEN_SSL_PATH=$(PROJ_PATH)/dep/openssl-1.0.2q
OPEN_SSL_INC=$(PROJ_PATH)/dep/openssl-1.0.2q/include

CURL_PATH=$(PROJ_PATH)/dep/curl-7.63.0
CURL_INC=$(CURL_PATH)/include

JSON_PATH=$(PROJ_PATH)/dep/jsoncpp-src-0.5.0
JSON_INC=$(JSON_PATH)/include

XML_PATH=$(PROJ_PATH)/dep/tinyxml2
XML_INC=$(PROJ_PATH)/dep/tinyxml2

SPD_LOG_PATH=$(PROJ_PATH)/dep/spdlog-1.x
SPD_LOG_INC=$(PROJ_PATH)/dep/spdlog-1.x/include

LIBCO_PATH=$(PROJ_PATH)/dep/libco
LIBCO_INC=$(PROJ_PATH)/dep/libco

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
