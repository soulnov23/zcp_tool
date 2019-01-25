include ../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN := test example_xml_parser example_json_parser

INCLUDE += -I$(CURL_INC) -I$(JSON_INC) -I$(XML_INC) -I$(OPEN_SSL_INC) -I$(SPD_LOG_INC) -I$(LIBCO_INC) -I$(COMM_PATH)
LDFLAGS += -L$(LIB_DIR) -lcurl -ljson -ltinyxml2 -lcrypto -lssl -lcolib -lcomm

all : clean $(BIN)

test : test.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_xml_parser : example_xml_parser.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_json_parser : example_json_parser.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

clean :
	rm -rf $(OBJS) $(BIN)
