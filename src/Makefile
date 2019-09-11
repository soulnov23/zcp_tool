include ../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN = $(BIN_DIR)/zcp_server

INCLUDE += -I$(COMM_INC) -I$(COMM_CRYPTO_INC) -I$(COMM_NET_INC) -I$(XML_INC) -I$(JSON_CPP_INC) -I$(RAPID_JSON_INC) \
           -I$(OPEN_SSL_INC) -I$(CURL_INC) -I$(LIBCO_INC)
LDFLAGS += -L$(LIB_DIR) -lpthread -ltinyxml2 -ljson -lcrypto -lcurl -laio -lcolib -lcomm -lcomm_crypto -lcomm_net -ltcmalloc_minimal

all : clean $(BIN)

$(BIN) : $(OBJS)
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

clean :
	rm -rf $(OBJS) $(BIN) *.o