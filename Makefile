CFLAGS = -Wall -Wextra -g -std=c99 -O3

all: build/6502_emu.o build/cpu.o
	gcc ${CFLAGS} -o emulator build/6502_emu.o build/cpu.o

build/6502_emu.o: src/6502_emu.c build/cpu.o
	gcc -c ${CFLAGS} src/6502_emu.c -o build/6502_emu.o

build/cpu.o: src/cpu.c src/cpu.h src/types.h build/memory.gch build/instruction.gch
	gcc -c ${CFLAGS} src/cpu.c -o build/cpu.o

build/memory.gch: src/6502_memory.h
	gcc ${CFLAGS} src/6502_memory.h -o build/memory.gch

build/instruction.gch: src/instruction.h
	gcc ${CFLAGS} src/instruction.h -o build/instruction.gch

test_cpu: spec/6502_emu_spec.c
	gcc -g -o test_cpu_spec spec/6502_emu_spec.c && ./test_cpu_spec ; rm test_cpu_spec

clean:
	rm build/*
