include ../../Inc.mk

BIN = $(BIN_DIR)/zcp_tool

CFLAGS   +=
CXXFLAGS +=
INCLUDE  +=
LDFLAGS  += -lbase -lyaml-cpp -lpthread

all: $(BIN)

#链接顺序：目标文件在前、库在后
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic $^ $(LDFLAGS) -o $@

clean:
	rm -rf $(OBJS) $(DEPS) $(BIN)

format:
	$(if $(FORMAT_FILES),$(FORMAT) $(FORMAT_FILES))

.PHONY: all clean format

.DEFAULT_GOAL := all
