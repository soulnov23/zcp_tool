include ../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN = $(BIN_DIR)/zcp_server

CFLAGS   +=
CXXFLAGS +=
INCLUDE  +=
LDFLAGS  += -lpthread -L$(LIB_DIR) -lcurl -lfmt -lssl -lcrypto \
		    -lpicohttpparser -ltinyxml2 -lyaml -lbase

all: $(BIN)
	
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(OBJS) $(BIN)

format:
	$(FORMAT) ./*.cpp ./*.h

.PHONY: all clean format

.DEFAULT_GOAL: all