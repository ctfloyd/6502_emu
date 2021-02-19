#include "cpu.h"
#include "flags.h"
#include "types.h"

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

enum Instruction {
    LDA_IMM = 0xA9,
    LDA_ZERO = 0xA5,
    LDA_ZERO_X = 0xB5,
    LDA_ABS = 0xAD
};

static inline int lda_imm(CPU* cpu) {
    // Sets zero and negative flags
    Byte accumulator_byte = cpu_load_next_byte(cpu);
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 2;
}

static inline int lda_zero(CPU* cpu) {
    Byte zero_page_addr = cpu_load_next_byte(cpu);
    Byte accumulator_byte = cpu->memory.data[zero_page_addr];
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 3;
}

static inline int lda_zero_x(CPU* cpu) {
    Byte zero_page_addr = cpu_load_next_byte(cpu);
    Byte offset = cpu->idx_reg_x;

    // The size of Byte is u8, so this wraps at 255
    Byte effective_addr = zero_page_addr + offset;
    Byte accumulator_byte = cpu->memory.data[effective_addr];
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 4;
}

static inline int lda_absolute(CPU* cpu) {
    Word effective_addr = cpu_load_next_word(cpu);
    Byte accumulator_byte = cpu->memory.data[effective_addr];
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 4;
}

#endif

