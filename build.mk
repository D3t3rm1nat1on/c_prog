CFLAGS += $(INCLUDES)

# $1=test_name (shared_mutex ...)
define test_rule

-include $($1_src_objs:.o=.d)
-include $(deps_dir)/$1.d

$(build_root)/%.o: $$($1_src_root)/%.c $(deps_dir)/%.d | $(deps_dir)
	$(CC) $(CFLAGS) -c -o $$@ $$<

$(build_root)/%.dump: (build_root)/%.o
	objdump -Dz $$< > $$@

$(build_root)/$1: $(build_root)/$1.o $$($1_src_objs) $(lib_objs) 
	$(CC) $(CFLAGS) -o $$@ $$^

$1: $(build_root)/$1 | $(build_root)
	cp $$< $$@
endef

$(foreach test,$(TESTS),$(eval $(call test_rule,$(test))))
