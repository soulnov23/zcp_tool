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
ARFLAGS = -rcsD
RANLIB = ranlib -D

CFLAGS 	 ?=
CXXFLAGS ?=
INCLUDE  ?=
LDFLAGS  ?=

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

PROJ_PATH := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
BIN_DIR = $(PROJ_PATH)/release/bin
LIB_DIR = $(PROJ_PATH)/release/lib

#由于第三方库代码里面include的路径就是按照原目录结构，导致引用第三方库不能直接使用全路径
FMT_INC 			= $(PROJ_PATH)/third_party/fmt/include
GTEST_INC			= $(PROJ_PATH)/third_party/googletest/googletest/include
GMOCK_INC			= $(PROJ_PATH)/third_party/googletest/googlemock/include
JSONCPP_INC 		= $(PROJ_PATH)/third_party/jsoncpp/include
LIBUUID_INC			= $(PROJ_PATH)/third_party/libuuid/include
PICOHTTPPARSER_INC 	= $(PROJ_PATH)/third_party/picohttpparser/include
RAPIDJSON_INC		= $(PROJ_PATH)/third_party/rapidjson/include
SPDLOG_INC 			= $(PROJ_PATH)/third_party/spdlog/include
TINYXML2_INC 		= $(PROJ_PATH)/third_party/tinyxml2/include
YAML_INC 			= $(PROJ_PATH)/third_party/yaml/include
OAUTH_INC			= $(PROJ_PATH)/third_party/oauth/include

INCLUDE += -I$(PROJ_PATH) -I$(FMT_INC) -I$(GTEST_INC) -I$(GMOCK_INC) \
		   -I$(JSONCPP_INC) -I$(LIBUUID_INC) -I$(PICOHTTPPARSER_INC) \
		   -I$(RAPIDJSON_INC) -I$(SPDLOG_INC) -I$(TINYXML2_INC) -I$(YAML_INC) -I$(OAUTH_INC)

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