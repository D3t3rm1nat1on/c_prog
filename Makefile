.PHONY: default all list clean help

WARNINGS=-Wall -Werror

lib_dir=lib
lib_object_dir=objs

deps_dir=.deps
tests_dir=tests
build_root=build
CC=gcc
CFLAGS=-I$(lib_dir) $(WARNINGS)

lib_src=$(shell find $(lib_dir) -name *.c -printf "%f\n")
lib_objs=$(patsubst %, $(lib_object_dir)/%, $(lib_src:.c=.o))
lib_dump=$(patsubst %, $(lib_object_dir)/%, $(lib_src:.c=.dump))
lib_deps:=$(patsubst %, $(deps_dir)/%, $(lib_src:.c=.d))

TESTS = $(shell find $(tests_dir)/* -maxdepth 1 -type d -printf "%f ")

.PRECIOUS: $(deps_dir)/%.d

default: all

all: $(TESTS)

define include_test
include $(tests_dir)/$1/Makefile
endef

$(foreach test,$(TESTS),$(eval $(call include_test,$(test))))

list: 
	@ echo "available tests --> $(TESTS)"

help:
	@ echo "make <test_name> --> make msg"
	@ echo "make list (get tests list)"
	@ echo "make clean"
	@ echo "make (build all tests)"

clean:
	rm -f *.out objs/* build/* .deps/* $(TESTS)

$(lib_object_dir):
	mkdir -p $@

$(deps_dir):
	mkdir -p $@

$(build_root):
	mkdir -p $@

-include $(lib_deps)

$(lib_object_dir)/%.o: $(lib_dir)/%.c $(lib_deps) | $(lib_object_dir) $(deps_dir)
	$(CC) $(CFLAGS) -c $< -o $@

$(lib_object_dir)/%.dump: $(lib_object_dir)/%.o
	objdump -Dz $< > $@

$(deps_dir)/%.d: $(lib_dir)/%.c | $(deps_dir)
	$(CC) $(CFLAGS) -MM -MT $(patsubst $(deps_dir)/%.d,$(lib_object_dir)/%.o,$@) $< > $@

include build.mk

