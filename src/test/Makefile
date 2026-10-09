include ../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN := $(patsubst %.cpp, ./bin/%, $(SRC))

CFLAGS   += -Wno-class-memaccess
CXXFLAGS += -Wno-class-memaccess
INCLUDE  += 
LDFLAGS  += -L$(LIB_DIR) -lbase -lgtest_main -lgtest -ltinyxml2 -lyaml-cpp -lpthread

all: check $(BIN)

#一条静态模式规则覆盖所有用例，新增.cpp无需再加规则
#（原先12条规则逐个手写，漏写就会报 No rule to make target）
#链接顺序：目标文件在前、库在后
$(BIN): ./bin/%: %.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $< $(LDFLAGS) -o $@

clean:
	rm -rf $(OBJS) $(DEPS) $(BIN)

format:
	$(FORMAT) ./*.cpp ./*.h

check:
	mkdir -p bin

.PHONY: all clean format check

.DEFAULT_GOAL := all