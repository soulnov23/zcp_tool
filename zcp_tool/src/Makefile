include ../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN = $(BIN_DIR)/zcp_server

CFLAGS   +=
CXXFLAGS +=
INCLUDE  += -I$(PROJ_PATH)
LDFLAGS  += -L$(LIB_DIR) -lcurl -lfmt -lssl -lcrypto -lpicohttpparser \
		    -ltinyxml2 -lyaml -lcomm -lcomm_crypto -lcomm_net -lpthread

all: $(BIN)
	
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(OBJS) $(BIN)

format:
	$(FORMAT) ./*.cpp ./*.h