SRC_DIR=./src
OBJ_DIR=./obj
BIN_DIR?=./bin
INC_DIR=-I./include

NAME=mzapo_sdl
BINARY=$(BIN_DIR)/lib$(NAME).so

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CFLAGS=$(INC_DIR) -Wall -Werror -g -O0
CFLAGS+=$(shell sdl2-config --cflags)

.PHONY: all clean

all: $(BINARY)
	cp img/mzapo.png $(BIN_DIR)

$(BINARY): $(OBJ) | $(BIN_DIR)
	$(CC) -shared $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	echo $(BIN_DIR)
	mkdir -p $@

dep: depend

clean:
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR)
	$(RM) -rv .cache

-include $(OBJ:.o=.d)
