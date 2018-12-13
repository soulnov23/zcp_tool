CC = gcc
CXX = g++

#==============================================================================
#	BUILD:	编译类型
#------------------------------------------------------------------------------
#		BUILD_DEV:			开发版本
#		BUILD_DEBUG:		测试版本
#		BUILD_NORMAL:		一般开发版本
#		BUILD_RELEASE:		发行版本
#------------------------------------------------------------------------------
#BUILD=BUILD_NORMAL
BUILD=BUILD_DEV

ifeq ($(BUILD), BUILD_DEV)
CFLAGS =  -Wall -fPIC -g -DDEBUG
endif
ifeq ($(BUILD), BUILD_DEBUG)
CFLAGS =  -Wall -fPIC -g -DDEBUG
endif
ifeq ($(BUILD), BUILD_NORMAL)
CFLAGS =  -Wall -fPIC -g -DINFO
endif
ifeq ($(BUILD), BUILD_RELEASE)
CFLAGS =  -Wall -fPIC -O2 -g -DINFO
endif

PROJ_PATH=/home/zcp_tools
PROJ_LIB=$(PROJ_PATH)/lib
SCONS_PATH=$(PROJ_PATH)/dep/scons-2.1.0

CURL_PATH=$(PROJ_PATH)/dep/curl-7.63.0
CURL_INC=$(CURL_PATH)/include

JSON_PATH=$(PROJ_PATH)/dep/jsoncpp-src-0.5.0
JSON_INC=$(JSON_PATH)/include