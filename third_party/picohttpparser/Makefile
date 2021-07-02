include ../../Inc.mk

OBJS := picohttpparser.o
SO_NAME = libpicohttpparser.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libpicohttpparser.a
A_BIN = $(LIB_DIR)/$(A_NAME)

INCLUDE +=
LDFLAGS +=

all: $(SO_BIN) $(A_BIN)

$(SO_BIN): $(OBJS)
	$(CC) $(CXXFLAGS) -shared -Wl,-soname,$(SO_NAME) $(LDFLAGS) $^ -o $@

$(A_BIN): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

clean:
	rm -rf $(OBJS) $(SO_BIN) $(A_BIN)