include ../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
SO_NAME = libcomm.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libcomm.a
A_BIN = $(LIB_DIR)/$(A_NAME)

INCLUDE += -I$(XML_PATH) -I$(JSON_INC) -I$(OPEN_SSL_INC) -I$(CURL_INC)
LDFLAGS += -lpthread -L$(LIB_DIR) -ltinyxml2 -ljson -lcrypto -lcurl

all : clean $(SO_BIN) $(A_BIN)

$(SO_BIN) : $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,$(SO_NAME) -o $@ $^ $(LDFLAGS)

$(A_BIN) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

clean :
	rm -rf $(OBJS) $(SO_BIN) $(A_BIN)
