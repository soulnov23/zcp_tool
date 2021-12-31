include ../../Inc.mk

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
SO_NAME = libbase.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libbase.a
A_BIN = $(LIB_DIR)/$(A_NAME)

CFLAGS   +=
CXXFLAGS +=
INCLUDE  +=
LDFLAGS  += -ldl -lpthread -lssl -lcrypto -lcurl -L$(LIB_DIR) -lfmt  \
		    -lpicohttpparser -ltinyxml2 -lyaml -ljsoncpp -luuid

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

.PHONY: all clean format

.DEFAULT_GOAL: all
