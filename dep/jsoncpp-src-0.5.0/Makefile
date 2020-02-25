include ../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
SO_NAME = libjson.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libjson.a
A_BIN = $(LIB_DIR)/$(A_NAME)

INCLUDE += -I$(JSON_CPP_INC)
LDFLAGS +=

all: clean $(SO_BIN) $(A_BIN)

$(SO_BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,$(SO_NAME) -o $@ $^ $(LDFLAGS)

$(A_BIN): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

clean:
	rm -rf $(OBJS)