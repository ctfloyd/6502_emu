#include <stdbool.h>
#include "types.h"
#include "6502_memory.h"

#ifndef CPU_H
#define CPU_H

struct Flags {
	bool carry: 1;
	bool zero: 1;
	bool interrupt_disable: 1;
	bool decimal_mode: 1;
	bool break_command: 1;
	bool overflow: 1;
	bool negative: 1;
};

typedef struct CPU {
	Word program_counter;
	Byte stack_pointer;
	Byte accumulator;
	Byte idx_reg_x;
	Byte idx_reg_y;
	struct Flags flags;
    Memory memory;
} CPU;

CPU* cpu_create(int);
void cpu_dump_state(CPU*);
Byte cpu_load_next_byte(CPU*);
void cpu_reset(CPU*);
void cpu_run(CPU*, int);

#endif