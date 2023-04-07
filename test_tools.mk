$(TEST_NAME)_src_root := $(tests_dir)/$(TEST_NAME)/
$(TEST_NAME)_src := $(shell find $($(TEST_NAME)_src_root) -name *.c -printf "%f\n")
$(TEST_NAME)_src_objs := $(patsubst %, $(build_root)/%, $($(TEST_NAME)_src:.c=.o))

$(deps_dir)/%.d: $($(TEST_NAME)_src_root)/%.c | $(deps_dir)
	$(CC) $(CFLAGS) -MM -MT $(patsubst $(deps_dir)/%.d,$(build_root)/%.o,$@) $< > $@

