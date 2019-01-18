include ../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN = $(BIN_DIR)/main

INCLUDE += -I$(CURL_INC) -I$(JSON_INC) -I$(XML_INC) -I$(OPEN_SSL_INC) -I$(SPD_LOG_INC) -I$(COMM_PATH)
LDFLAGS += -L$(LIB_DIR) -lcurl -ljson -ltinyxml2 -lcrypto -lssl -lcomm

all : clean $(BIN)

$(BIN) : $(OBJS)
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

clean :
	rm -rf $(OBJS) $(BIN)