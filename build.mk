CFLAGS += $(INCLUDES)

src_objs=$(patsubst %, $(build_root)/%, $(src:.c=.o))

$(info src=$(src))
$(info src_objs=$(src_objs))

$(info "$(build_root)/%.o: $(test_src_root)/%.c $(build_root)")
$(build_root)/%.o: $(test_src_root)/%.c $(build_root)
	$(CC) $(CFLAGS) -c $< -o $@

(build_root)/%.dump: (build_root)/%.o
	objdump -Dz $< > $@

$(info "$(build_root)/$(TEST_NAME): $(build_root)/$(TEST_NAME).o $(src_objs)")
$(build_root)/$(TEST_NAME): $(build_root)/$(TEST_NAME).o $(src_objs)
	$(CC) $(CFLAGS) -o $@ $^ objs/*.o

%: $(build_root)/%
	mv $< $@

$(build_root):
	mkdir $(build_root)

