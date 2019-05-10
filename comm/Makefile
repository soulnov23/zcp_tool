include ../Inc.mk

DIR_MODULES=crypto

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
SO_NAME = libcomm.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libcomm.a
A_BIN = $(LIB_DIR)/$(A_NAME)

INCLUDE += -I$(XML_INC) -I$(JSON_CPP_INC) -I$(RAPID_JSON_INC) -I$(OPEN_SSL_INC) -I$(CURL_INC) -I$(LIBCO_INC)
LDFLAGS += -L$(LIB_DIR) -lpthread -ltinyxml2 -ljson -lcrypto -lcurl -laio -lcolib -ltcmalloc_minimal

all : clean $(SO_BIN) $(A_BIN)
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir; \
		echo; \
	done

$(SO_BIN) : $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,$(SO_NAME) -o $@ $^ $(LDFLAGS)

$(A_BIN) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

clean :
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir clean; \
		echo; \
	done
	rm -rf $(OBJS) $(SO_BIN) $(A_BIN)
