#include "cpu.h"
#include "stdio.h"
#include "instruction.h"

int main() {
	CPU* cpu = cpu_create(32);
	cpu_reset(cpu);
	cpu_dump_state(cpu);
    printf("Size of flags is: %ld\n", sizeof(struct Flags));

	cpu->memory.data[0] = LDA_IMM;
	cpu->memory.data[1] = 0X69;
	
	cpu_run(cpu, 1);
}
