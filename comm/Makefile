CC = g++
TARGET_CXXFLAGS = -g -Wall
TARGET_INCLUDE = -I/usr/local/include
MODULE := lib

SRC := $(wildcard *.cpp)
OBJ := $(patsubst %.cpp, %.o, $(SRC))

all : $(MODULE)
	@echo "compile lib!"

$(MODULE) : $(OBJ)
	@cp $(OBJ) $(OBJ_DIR)

%.o : %.cpp
	$(CC) $(TARGET_CXXFLAGS) -c $^ $(TARGET_INCLUDE)

.PHONY : clean

clean :
	rm -rf $(OBJ) *.bak