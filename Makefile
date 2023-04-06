.PHONY: default all list clean help

WARNINGS=-Wall -Werror

lib_dir=lib
lib_object_dir=objs

export tests_dir=tests
export build_root=build
export CC=gcc
export CFLAGS=-I$(lib_dir) $(WARNINGS) 

lib_src=$(shell find $(lib_dir) -name *.c -printf "%f\n")
lib_objs=$(patsubst %, $(lib_object_dir)/%, $(lib_src:.c=.o))
lib_dump=$(patsubst %, $(lib_object_dir)/%, $(lib_src:.c=.dump))

TESTS = $(shell find $(tests_dir)/* -maxdepth 1 -type d -printf "%f ")

default: all

all: $(TESTS)

$(TESTS): $(lib_object_dir) $(lib_objs) $(lib_dump)
	@echo "----> $@: $^"
	$(MAKE) -f $(tests_dir)/$@/Makefile $@

list: 
	@ echo "available tests --> $(TESTS)"

help:
	@ echo "make <test_name> --> make msg"
	@ echo "make list (get tests list)"
	@ echo "make clean"
	@ echo "make (build all tests)"

clean:
	rm -f *.out objs/* build/* $(TESTS)

$(lib_object_dir):
	mkdir $(lib_object_dir)

$(lib_object_dir)/%.o: $(lib_dir)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(lib_object_dir)/%.dump: $(lib_object_dir)/%.o
	objdump -Dz $< > $@
