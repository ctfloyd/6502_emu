#include "cpu.h"
#include "flags.h"
#include "types.h"

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

enum Instruction {
    LDA_IMM = 0xA9,
    LDA_ZERO = 0xA5,
    LDA_ZERO_X = 0xB5,
    LDA_ABS = 0xAD,
    LDA_ABS_X = 0xBD,
    LDA_ABS_Y = 0xB9
};

static inline Byte load_zero_page_value(CPU* cpu, Byte offset) {
    Byte zero_page_addr = cpu_load_next_byte(cpu);
    // The size of Byte is u8, so this wraps at 255
    Byte effective_addr = zero_page_addr + offset;
    return cpu->memory.data[effective_addr];
}

static inline Byte load_absolute_value(CPU* cpu, Byte offset) {
    Word base_addr = cpu_load_next_word(cpu);
    Word effective_addr = base_addr + offset;
    return cpu->memory.data[effective_addr];
}

static inline int lda_imm(CPU* cpu) {
    // Sets zero and negative flags
    Byte accumulator_byte = cpu_load_next_byte(cpu);
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 2;
}

static inline int lda_zero(CPU* cpu) {
    Byte accumulator_byte = load_zero_page_value(cpu, 0);
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 3;
}

static inline int lda_zero_x(CPU* cpu) {
    Byte accumulator_byte = load_zero_page_value(cpu, cpu->idx_reg_x);
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 4;
}

static inline int lda_absolute(CPU* cpu) {
    Byte accumulator_byte = load_absolute_value(cpu, 0);
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 4;
}

/* TODO (ctfloyd) Check (and incrase cycle count) if load crosses page boundary */
static inline int lda_absolute_x(CPU* cpu) {
    Byte accumulator_byte = load_absolute_value(cpu, cpu->idx_reg_x);
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 4;
}

/* TODO (ctfloyd) Check (and incrase cycle count) if load crosses page boundary */
static inline int lda_absolute_y(CPU* cpu) {
    Byte accumulator_byte = load_absolute_value(cpu, cpu->idx_reg_y);
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 4;
}

#endif

