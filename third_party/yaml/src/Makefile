include ../../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
SO_NAME = libyaml.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libyaml.a
A_BIN = $(LIB_DIR)/$(A_NAME)

INCLUDE +=
LDFLAGS +=

all: $(SO_BIN) $(A_BIN)

#链接顺序：目标文件在前、库在后，否则静态库中的符号不会被解析
$(SO_BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,$(SO_NAME) $^ $(LDFLAGS) -o $@

#用$^而不是$?：$?只含比目标新的依赖，源文件删除后旧成员会永久留在.a里
$(A_BIN): $(OBJS)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

clean:
	rm -rf $(OBJS) $(DEPS) $(SO_BIN) $(A_BIN)

.PHONY: all clean

.DEFAULT_GOAL := all