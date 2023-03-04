.PHONY: default all

SOURCE_DIR=source
LIB_DIR=lib
OBJ_DIR=objs

SRC=$(shell find $(SOURCE_DIR) -name *.c -printf "%f\n")
OUT=$(SRC:.c=.out)
$(info OUT=$(OUT))

LIB_SRC=$(shell find $(LIB_DIR) -name *.c -printf "%f\n")
LIB=$(patsubst %, $(OBJ_DIR)/%, $(LIB_SRC:.c=.o))
$(info LIB=$(LIB))

CC=gcc
CFLAGS=-I. -I$(LIB_DIR) -Wall

default: all

all: $(OUT) $(LIB)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $^ -o $@

list_test.out: $(SOURCE_DIR)/list_test.c $(OBJ_DIR)/list.o
	$(CC) $(CFLAGS) -o $@ $^

%.out: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.out
