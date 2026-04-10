.PHONY: all
all: main_no_barriers.elf main_with_barriers.elf

main_no_barriers.elf: main.cpp
	g++ -O2 -pthread -o $@ $<

main_with_barriers.elf: main.cpp
	g++ -O2 -pthread -DUSE_BARRIERS -o $@ $<

.PHONY: run
run: all
	@echo "=== Without barriers ==="
	./main_no_barriers.elf 500000
	@echo "=== With barriers ==="
	./main_with_barriers.elf 500000

.PHONY: clean
clean:
	-rm -f main_no_barriers.elf main_with_barriers.elf
