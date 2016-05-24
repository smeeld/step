SUB=server sender
all: $(SUB)
$(SUB):
	$(MAKE) -C $@
.PHONY: $(SUB)
clean:
	for x in $(SUB); do \
	make clean -C $$x; \
	done
	rm servd send
