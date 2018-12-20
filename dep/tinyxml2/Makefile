include ../../Inc.mk

OBJS := tinyxml2.o
SO_NAME = libtinyxml2.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libtinyxml2.a
A_BIN = $(LIB_DIR)/$(A_NAME)

INCLUDE +=
LDFLAGS +=

all : clean $(SO_BIN) $(A_BIN)

$(SO_BIN) : $(OBJS)
	$(CXX) -z defs -shared -Wl,-soname,$(SO_NAME) -o $@ $^ $(LDFLAGS)
	
$(A_BIN) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

clean :
	rm -rf $(OBJS)