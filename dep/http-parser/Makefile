include ../../Inc.mk

OBJS := http_parser.o
SO_NAME = libhttp_parser.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libhttp_parser.a
A_BIN = $(LIB_DIR)/$(A_NAME)

INCLUDE +=
LDFLAGS +=

install: $(SO_BIN) $(A_BIN)

$(SO_BIN): $(OBJS)
	$(CC) $(CXXFLAGS) -shared -Wl,-soname,$(SO_NAME) -o $@ $^ $(LDFLAGS)

$(A_BIN): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

uninstall:
	rm -rf $(OBJS) $(SO_BIN) $(A_BIN)