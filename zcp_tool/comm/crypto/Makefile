include ../../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
SO_NAME = libcomm_crypto.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libcomm_crypto.a
A_BIN = $(LIB_DIR)/$(A_NAME)

INCLUDE +=
LDFLAGS += -lpthread -ltinyxml2 -lpicohttpparser -lcrypto -lcurl \
		   -laio -lcomm -lssl -lfmt

all: $(SO_BIN) $(A_BIN)
	
$(SO_BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -Wl,-rpath=$(LIB_DIR) -Wl,-soname,$(SO_NAME) $(LDFLAGS) $^ -o $@

$(A_BIN): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

clean:
	rm -rf $(OBJS) $(SO_BIN) $(A_BIN)

format:
	$(FORMAT) ./*.cpp ./*.h