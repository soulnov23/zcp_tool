include ../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN = $(BIN_DIR)/zcp_tool

CFLAGS   +=
CXXFLAGS +=
INCLUDE  +=
LDFLAGS  += -L$(LIB_DIR) -lbase -lfmt -ljsoncpp -lpicohttpparser -lspdlog -ltinyxml2 -luuid -lyaml \
			-ldl -lpthread -lssl -lcrypto -lcurl

all: $(BIN)

#链接顺序：目标文件在前、库在后
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $^ $(LDFLAGS) -o $@

clean:
	rm -rf $(OBJS) $(DEPS) $(BIN)

format:
	$(FORMAT) ./*.cpp ./*.h

.PHONY: all clean format

.DEFAULT_GOAL := all