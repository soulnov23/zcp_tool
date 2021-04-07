include ../../Inc.mk

DIR_MODULES = crypto \
			  net

SRC := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRC))
SO_NAME = libcomm.so
SO_BIN = $(LIB_DIR)/$(SO_NAME)
A_NAME = libcomm.a
A_BIN = $(LIB_DIR)/$(A_NAME)

INCLUDE += -I$(PROJ_PATH)
LDFLAGS += -L$(LIB_DIR) -lpthread -ldl -ltinyxml2 -lhttp_parser -lcrypto -lcurl \
		   -laio -lcolib -ltcmalloc_minimal -lssl

all: $(SO_BIN) $(A_BIN)
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir; \
		echo; \
	done

$(SO_BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -shared -Wl,-rpath=$(LIB_DIR) -Wl,-soname,$(SO_NAME) $(LDFLAGS) $^ -o $@

$(A_BIN): $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
	$(RANLIB) $@

clean:
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir clean; \
		echo; \
	done
	rm -rf $(OBJS) $(SO_BIN) $(A_BIN)

format:
	@for dir in $(DIR_MODULES); \
	do \
		make -C $$dir format; \
		echo; \
	done
	$(FORMAT) ./*.cpp ./*.h
