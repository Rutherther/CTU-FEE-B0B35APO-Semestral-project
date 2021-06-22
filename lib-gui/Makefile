SRC_DIR=./src
OBJ_DIR=./obj
BIN_DIR?=./bin
INC_DIR=-I./include $(INHERIT_INCLUDES)

ifdef COMPUTER
	CC = clang
else
	CC = arm-linux-gnueabihf-gcc
	CXX = arm-linux-gnueabihf-g++
endif

CPPFLAGS = -I .
CFLAGS =-g -std=gnu99 -O1 -Wall -D ILI9481
CXXFLAGS = -g -std=gnu++11 -O1 -Wall

ifdef COMPUTER
CFLAGS += -DCOMPUTER
else
endif

NAME=mzapo-gui
BINARY=$(BIN_DIR)/lib$(NAME).so

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CFLAGS+=$(INC_DIR) -Wall -Werror
CFLAGS+=$(shell sdl2-config --cflags)

.PHONY: all clean

all: $(BINARY)

$(BINARY): $(OBJ) | $(BIN_DIR)
	$(CC) -shared $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	echo $(BIN_DIR)
	mkdir -p $@
clean:
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR)
	$(RM) -rv .cache
	$(RM) -rv connect.gdb
	$(RM) -rv depend

-include $(OBJ:.o=.d)
