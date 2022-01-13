include ../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
BIN := $(patsubst %.cpp, ./bin/%, $(SRC))

CFLAGS   += -Wno-class-memaccess
CXXFLAGS += -Wno-class-memaccess
INCLUDE  +=
LDFLAGS  += -ldl -lpthread -lssl -lcrypto -lcurl \
			-L$(LIB_DIR) -lbase -lfmt -lgtest_main -lgtest -lgmock -ljsoncpp -lpicohttpparser -lspdlog -ltinyxml2 -luuid -lyaml

all: check $(BIN)
	
./bin/test: test.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/client: client.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/server: server.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_rapid_json: example_rapid_json.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_curl: example_curl.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_epoll: example_epoll.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_graph: example_graph.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/example_coroutine: example_coroutine.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/map_util_test: map_util_test.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/xml_parser_test: xml_parser_test.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

./bin/log_test: log_test.o
	$(CXX) $(CXXFLAGS) -Wl,--export-dynamic -Wl,-rpath=$(LIB_DIR) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(OBJS) $(BIN)

format:
	$(FORMAT) ./*.cpp ./*.h

check: 
	mkdir -p bin

.PHONY: all clean format check

.DEFAULT_GOAL: all
	