include ../../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
SO_NAME = libyaml.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libyaml.a
A_BIN = $(LIB_DIR)/$(A_NAME)

INCLUDE +=
LDFLAGS +=

all: $(SO_BIN) $(A_BIN)

$(SO_BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,$(SO_NAME) $(LDFLAGS) $^ -o $@

$(A_BIN): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

clean:
	rm -rf $(OBJS) $(SO_BIN) $(A_BIN)

.PHONY: all clean

.DEFAULT_GOAL: all