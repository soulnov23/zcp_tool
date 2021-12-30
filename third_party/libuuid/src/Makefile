include ../../../Inc.mk

SRC := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRC))
SO_NAME = libuuid.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libuuid.a
A_BIN = $(LIB_DIR)/$(A_NAME)

INCLUDE +=
LDFLAGS +=

all: $(SO_BIN) $(A_BIN)

$(SO_BIN): $(OBJS)
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(SO_NAME) $(LDFLAGS) $^ -o $@

$(A_BIN): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

clean:
	rm -rf $(OBJS) $(SO_BIN) $(A_BIN)

.PHONY: all clean

.DEFAULT_GOAL: all