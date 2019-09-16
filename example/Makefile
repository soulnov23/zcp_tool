include ../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN := $(patsubst %.cpp, %, $(SRC))
#BIN := test client server example_xml_parser example_json_parser example_rapid_json example_curl example_libco example_epoll example_auto example_lambda

INCLUDE += -I$(COMM_INC) -I$(XML_INC) -I$(JSON_CPP_INC) -I$(RAPID_JSON_INC) \
           -I$(OPEN_SSL_INC) -I$(CURL_INC) -I$(LIBCO_INC)
LDFLAGS += -L$(LIB_DIR) -lpthread -ltinyxml2 -ljson -lcrypto -lcurl -laio -lcolib -lcomm -lcomm_crypto -lcomm_net -ltcmalloc_minimal

all : clean $(BIN)

test : test.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

client : client.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

server : server.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_xml_parser : example_xml_parser.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_json_parser : example_json_parser.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_rapid_json : example_rapid_json.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_curl : example_curl.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_libco : example_libco.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_epoll : example_epoll.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_auto : example_auto.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_lambda : example_lambda.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

clean :
	rm -rf $(OBJS) $(BIN) *.o