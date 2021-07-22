include ../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN = $(BIN_DIR)/zcp_server

INCLUDE +=
LDFLAGS += -lpthread -ltinyxml2 -lpicohttpparser -lcrypto -lcurl \
		   -laio -lcomm -lcomm_crypto -lcomm_net -lssl -lfmt

all: $(BIN)
	
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(OBJS) $(BIN)

format:
	$(FORMAT) ./*.cpp ./*.h