.PHONY: all
all: main_no_barriers.elf main_with_barriers.elf flag_data_no_barriers.elf flag_data_with_barriers.elf

main_no_barriers.elf: main.cpp
	g++ -O2 -pthread -o $@ $<

main_with_barriers.elf: main.cpp
	g++ -O2 -pthread -DUSE_BARRIERS -o $@ $<

flag_data_no_barriers.elf: flag_data.cpp
	g++ -O2 -pthread -o $@ $<

flag_data_with_barriers.elf: flag_data.cpp
	g++ -O2 -pthread -DUSE_BARRIERS -o $@ $<

.PHONY: run
run: all
	@echo "=== Dekker test: without barriers ==="
	./main_no_barriers.elf 500000
	@echo "=== Dekker test: with barriers ==="
	./main_with_barriers.elf 500000
	@echo "=== Flag+data test: without barriers ==="
	./flag_data_no_barriers.elf 500000
	@echo "=== Flag+data test: with barriers ==="
	./flag_data_with_barriers.elf 500000

.PHONY: clean
clean:
	-rm -f main_no_barriers.elf main_with_barriers.elf flag_data_no_barriers.elf flag_data_with_barriers.elf
