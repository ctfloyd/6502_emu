#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"
#include "instruction.h"


CPU* cpu_create(int memory_size) {
    CPU* cpu = malloc(sizeof(CPU));
	Memory memory = { malloc(memory_size), memory_size };

    cpu->memory = memory;
    return cpu;
}

void cpu_destroy(CPU* cpu) {
	free(cpu->memory.data);
	free(cpu);
	cpu = NULL;
}

void cpu_dump_state(CPU* cpu) {
    printf("\n******Registers******\n");
	printf("PC: %d, SP: %d, A: %d\n X: %d,  Y: %d\n", 
			cpu->program_counter, 
			cpu->stack_pointer, 
			cpu->accumulator, 
			cpu->idx_reg_x, 
			cpu->idx_reg_y
		  );
	
	struct Flags* flags = &cpu->flags;
	printf("\n******Flags******\n");
	printf(" C: %d,  Z: %d, ID: %d\nDM: %d, BC: %d,  O: %d\n N: %d\n",
			flags->carry,
			flags->zero,
			flags->interrupt_disable,
			flags->decimal_mode,
			flags->break_command,
			flags->overflow,
			flags->negative
		  );
}

void cpu_reset(CPU* cpu) {
	cpu->program_counter = 0;
	cpu->stack_pointer = 0;
	cpu->accumulator = 0;
	cpu->idx_reg_x = 0;
	cpu->idx_reg_y = 0;
	cpu->flags.carry = 0;
	cpu->flags.zero = 0;
	cpu->flags.interrupt_disable = 0;
	cpu->flags.decimal_mode = 0;
	cpu->flags.break_command = 0;
	cpu->flags.overflow = 0;
	cpu->flags.negative = 0;

    for(int i = 0; i < cpu->memory.SIZE_IN_BYTES; i++) {
		cpu->memory.data[i] = 0;
	}
}


Byte cpu_load_next_byte(CPU* cpu) {
    return cpu->memory.data[cpu->program_counter++];
}

// Loads the next byte (Little Endian)
Word cpu_load_next_word(CPU* cpu) {
	Byte lo = cpu_load_next_byte(cpu);
	Byte hi = cpu_load_next_byte(cpu);
	Word word = (hi << 8) | lo;
	return word;
}

#define CYCLE_COUNT(instr) {  \
							  int c = instr;\
							  cycles_completed += c;\
							  cycles -= c;\
							  break;\
						   }

int cpu_run(CPU* cpu, int cycles) {
	int cycles_completed = 0;

    while(cycles > 0) {

        Byte next_byte = cpu_load_next_byte(cpu);

        switch(next_byte) {
            case LDA_IMM: CYCLE_COUNT(lda_imm(cpu));
			case LDA_ZERO: CYCLE_COUNT(lda_zero(cpu));
			case LDA_ZERO_X: CYCLE_COUNT(lda_zero_x(cpu));
			case LDA_ABS: CYCLE_COUNT(lda_absolute(cpu));
			case LDA_ABS_X: CYCLE_COUNT(lda_absolute_x(cpu));
			case LDA_ABS_Y: CYCLE_COUNT(lda_absolute_y(cpu));
			case LDA_IND_X: CYCLE_COUNT(lda_indirect_x(cpu));
			case LDA_IND_Y: CYCLE_COUNT(lda_indirect_y(cpu));
			case LDX_IMM: CYCLE_COUNT(ldx_imm(cpu));
			case LDX_ZERO: CYCLE_COUNT(ldx_zero(cpu));
			default:
				cycles--;
        }
    }

	return cycles_completed;
}