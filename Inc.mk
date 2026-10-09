#==============================================================================
#       BUILD:              编译类型
#------------------------------------------------------------------------------
#       BUILD_DEBUG:        开发版本
#       BUILD_RELEASE:      发行版本
#------------------------------------------------------------------------------
BUILD = BUILD_DEBUG
#BUILD = BUILD_RELEASE

CC = gcc -std=c17
CXX = g++ -std=c++20
AR = ar
ARFLAGS = -rcsD

ifeq ($(BUILD), BUILD_DEBUG)
CFLAGS   += -Wall -ggdb3 -fPIC -pipe -DDEBUG
CXXFLAGS += -Wall -ggdb3 -fPIC -pipe -DDEBUG
endif
ifeq ($(BUILD), BUILD_RELEASE)
CFLAGS   += -Wall -g -fPIC -pipe -O3
CXXFLAGS += -Wall -g -fPIC -pipe -O3
endif

LDFLAGS += -Wl,-z,defs

PROJ_PATH := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
BIN_DIR = $(PROJ_PATH)/release/bin
LIB_DIR = $(PROJ_PATH)/release/lib

INCLUDE += -I$(PROJ_PATH) -I$(PROJ_PATH)/third_party/include

FORMAT = clang-format --style=file --fallback-style=none -i

DEPFLAGS := -MMD -MP

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(DEPFLAGS) -c $< -o $@
%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(DEPFLAGS) -c $< -o $@
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(DEPFLAGS) -c $< -o $@
%.o: %.S
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

DEPS := $(wildcard *.d)
-include $(DEPS)