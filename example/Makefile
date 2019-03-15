include ../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN := test example_xml_parser example_json_parser example_curl

INCLUDE += -I$(COMM_INC) -I$(COMM_CRYPTO_INC) -I$(COMM_NET_INC) -I$(XML_INC) -I$(JSON_CPP_INC) -I$(RAPID_JSON_INC) -I$(OPEN_SSL_INC) -I$(CURL_INC) -I$(LIBCO_INC)
LDFLAGS += -L$(LIB_DIR) -lpthread -ltinyxml2 -ljson -lcrypto -lcurl -laio -lcolib -lcomm -lcomm_crypto -lcomm_net

all : clean $(BIN)

test : test.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_xml_parser : example_xml_parser.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_json_parser : example_json_parser.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)
	
example_curl : example_curl.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

clean :
	rm -rf $(OBJS) $(BIN)
