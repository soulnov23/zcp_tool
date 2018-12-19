include ../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
SO_NAME = libcomm.so
BIN = $(LIB_DIR)/$(SO_NAME)

INCLUDE +=
LDFLAGS += -lpthread

all : clean $(BIN)

$(BIN) : $(OBJS)
	$(CXX) -z defs -shared -Wl,-soname,$(SO_NAME) -o $@ $^ $(LDFLAGS)

clean :
	rm -rf $(OBJS) $(BIN)