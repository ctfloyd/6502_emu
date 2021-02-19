
#include "types.h"
#include "6502_memory.h"
#include "flags.h"
#include <stdbool.h>

#ifndef CPU_H
#define CPU_H

typedef struct CPU {
	Word program_counter;
	Byte stack_pointer;
	Byte accumulator;
	Byte idx_reg_x;
	Byte idx_reg_y;
	Flags flags;
    Memory memory;
} CPU;

CPU* cpu_create(int);
void cpu_dump_state(CPU*);
Byte cpu_load_next_byte(CPU*);
void cpu_reset(CPU*);
int cpu_run(CPU*, int);

#endif