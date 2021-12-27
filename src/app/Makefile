include ../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN = $(BIN_DIR)/zcp_server

CFLAGS   +=
CXXFLAGS +=
INCLUDE  +=
LDFLAGS  += -lpthread -lssl -lcrypto -lcurl -L$(LIB_DIR) -lfmt  \
		    -lpicohttpparser -ltinyxml2 -lyaml -ljsoncpp -lbase

all: $(BIN)
	
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(OBJS) $(BIN)

format:
	$(FORMAT) ./*.cpp ./*.h

.PHONY: all clean format

.DEFAULT_GOAL: all