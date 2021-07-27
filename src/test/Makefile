include ../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN := $(patsubst %.cpp, ./bin/%, $(SRC))

CFLAGS   +=
CXXFLAGS +=
INCLUDE  +=
LDFLAGS  += -lpthread -L$(LIB_DIR) -lcurl -lfmt -lssl -lcrypto \
		    -lpicohttpparser -ltinyxml2 -lyaml -lbase

all: check $(BIN)
	
./bin/test: test.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/client: client.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/server: server.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_xml_parser: example_xml_parser.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_json_parser: example_json_parser.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_rapid_json: example_rapid_json.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_curl: example_curl.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_epoll: example_epoll.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_auto: example_auto.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_lambda: example_lambda.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_graph: example_graph.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(OBJS) $(BIN)

format:
	$(FORMAT) ./*.cpp ./*.h

check: 
	mkdir -p bin

.PHONY: all clean format check

.DEFAULT_GOAL: all
	