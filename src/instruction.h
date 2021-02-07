#include "cpu.h"

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

enum Instruction {
    LDA_IMM = 0xA9
};

static inline int lda_imm(CPU* cpu) {
    // Sets zero and negative flags
    Byte accumulator_byte = cpu_load_next_byte(cpu);
    cpu->accumulator = accumulator_byte;
    if(accumulator_byte == 0) {
        cpu->flags.zero = true;
    } else {
        cpu->flags.zero = false;
    }

    if((s8)accumulator_byte < 0) {
        cpu->flags.negative = true;
    } else {
        cpu->flags.negative = false;
    }

    return 2;
}

#endif

