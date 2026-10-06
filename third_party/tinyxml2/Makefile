include ../../Inc.mk

OBJS := tinyxml2.o
SO_NAME = libtinyxml2.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libtinyxml2.a
A_BIN = $(LIB_DIR)/$(A_NAME)

CFLAGS   +=
CXXFLAGS +=
INCLUDE  +=
LDFLAGS  +=

all: $(SO_BIN) $(A_BIN)

#链接顺序：目标文件在前、库在后，否则静态库中的符号不会被解析
$(SO_BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,$(SO_NAME) $^ $(LDFLAGS) -o $@

#用$^而不是$?：$?只含比目标新的依赖，源文件删除后旧成员会永久留在.a里
$(A_BIN): $(OBJS)
	rm -f $@
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -rf $(OBJS) $(DEPS) $(SO_BIN) $(A_BIN)

.PHONY: all clean

.DEFAULT_GOAL := all