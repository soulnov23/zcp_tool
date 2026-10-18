include ../../Inc.mk

BIN := $(patsubst %.cpp,%,$(SRC))

CFLAGS   += -Wno-class-memaccess
CXXFLAGS += -Wno-class-memaccess
INCLUDE  +=
LDFLAGS  += -lbase -lgtest_main -lgtest -lyaml-cpp -lpthread

all: $(BIN)

#链接顺序：目标文件在前、库在后
$(BIN): %: %.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic $< $(LDFLAGS) -o $@

clean:
	rm -rf $(OBJS) $(DEPS) $(BIN)

format:
	$(FORMAT) ./*.cpp ./*.h

.PHONY: all clean format

.DEFAULT_GOAL := all
