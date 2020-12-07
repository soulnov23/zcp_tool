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
BIN_DIR = $(PROJ_PATH)/release/bin
LIB_DIR = $(PROJ_PATH)/release/lib
OBJ_DIR = $(PROJ_PATH)/release/obj

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