include ../../Inc.mk

BIN := $(patsubst %.cpp,%,$(SRC))

CFLAGS   += -Wno-class-memaccess
CXXFLAGS += -Wno-class-memaccess
INCLUDE  +=
LDFLAGS  += -lbase -lgtest_main -lgtest -lyaml-cpp -lpthread

all: $(BIN)

#一条静态模式规则覆盖所有用例，新增.cpp无需再加规则
#（原先12条规则逐个手写，漏写就会报 No rule to make target）
#链接顺序：目标文件在前、库在后
$(BIN): %: %.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic $< $(LDFLAGS) -o $@

clean:
	rm -rf $(OBJS) $(DEPS) $(BIN)

format:
	$(FORMAT) ./*.cpp ./*.h

.PHONY: all clean format

.DEFAULT_GOAL := all
