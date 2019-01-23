include ../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN := example_xml_parser example_json_parser example_atomic_count

INCLUDE += -I$(CURL_INC) -I$(JSON_INC) -I$(XML_INC) -I$(OPEN_SSL_INC) -I$(SPD_LOG_INC) -I$(LIBCO_INC) -I$(COMM_PATH)
LDFLAGS += -L$(LIB_DIR) -lcurl -ljson -ltinyxml2 -lcrypto -lssl -lcolib -lcomm

all : clean $(BIN)

example_xml_parser : example_xml_parser.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_json_parser : example_json_parser.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

example_atomic_count : example_atomic_count.o
	$(CXX) $(CXXFLAGS) -Wl,-rpath=$(LIB_DIR) -o $@ $^ $(LDFLAGS)

clean :
	rm -rf $(OBJS) $(BIN)
