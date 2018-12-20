#==============================================================================
#	BUILD:	编译类型
#------------------------------------------------------------------------------
#		BUILD_DEV:			开发版本
#		BUILD_DEBUG:		测试版本
#		BUILD_NORMAL:		一般开发版本
#		BUILD_RELEASE:		发行版本
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
LDFLAGS ?=
INCLUDE ?=

ifeq ($(BUILD), BUILD_DEBUG)
CFLAGS   += -Wall -g -fPIC -DDEBUG 
CXXFLAGS += -Wall -g -fPIC -DDEBUG 
LDFLAGS  += 
endif
ifeq ($(BUILD), BUILD_DEV)
CFLAGS   += -Wall -g -fPIC -DDEBUG -Werror 
CXXFLAGS += -Wall -g -fPIC -DDEBUG -Werror
LDFLAGS  +=  
endif
ifeq ($(BUILD), BUILD_NORMAL)
CFLAGS   += -Wall -g -fPIC -O
CXXFLAGS += -Wall -g -fPIC -O
LDFLAGS  += 
endif
ifeq ($(BUILD), BUILD_RELEASE)
CFLAGS   += -Wall -g -fPIC -O3 
CXXFLAGS += -Wall -g -fPIC -O3 
LDFLAGS  +=  
endif

PROJ_PATH=/home/zcp_tools
BIN_DIR=$(PROJ_PATH)/bin
LIB_DIR=$(PROJ_PATH)/lib
OBJ_DIR=$(PROJ_PATH)/obj

COMM_PATH=$(PROJ_PATH)/comm

SCONS_PATH=$(PROJ_PATH)/dep/scons-2.1.0

CURL_PATH=$(PROJ_PATH)/dep/curl-7.63.0
CURL_INC=$(CURL_PATH)/include

JSON_PATH=$(PROJ_PATH)/dep/jsoncpp-src-0.5.0
JSON_INC=$(JSON_PATH)/include

XML_PATH=$(PROJ_PATH)/dep/tinyxml2

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