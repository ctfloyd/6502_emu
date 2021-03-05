#include "cpu.h"
#include "flags.h"
#include "types.h"

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

/* 
    NOTE (ctfloyd): I thought about abstracting the load_* instrtuctions into some load_register_* instructions.
         However, with the current implementation this will not allow us to have fine grain cycle count
         control on a situational basis.
*/

enum Instruction {
    LDA_IMM = 0xA9,
    LDA_ZERO = 0xA5,
    LDA_ZERO_X = 0xB5,
    LDA_ABS = 0xAD,
    LDA_ABS_X = 0xBD,
    LDA_ABS_Y = 0xB9,
    LDA_IND_X = 0xA1,
    LDA_IND_Y = 0xB1,
    LDX_IMM = 0xA2,
    LDX_ZERO = 0xA6,
    LDX_ZERO_Y = 0xB6,
    LDX_ABS = 0xAE,
    LDX_ABS_Y = 0xBE
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

static inline Byte load_indexed_indirect(CPU* cpu, Byte offset) {
    Byte begin_byte = cpu_load_next_byte(cpu);
    Byte indirect_addr = begin_byte + offset;
    Byte effective_addr = cpu->memory.data[indirect_addr];
    return cpu->memory.data[effective_addr];
}

static inline Byte load_indexed_indirect_x(CPU* cpu) {
    return load_indexed_indirect(cpu, cpu->idx_reg_x);
}

static inline Byte load_indirect_indexed(CPU* cpu, Byte offset) {
    Byte begin_byte = cpu_load_next_byte(cpu);
    Byte effective_addr = cpu->memory.data[begin_byte] + offset;
    return cpu->memory.data[effective_addr];
}

static inline Byte load_indirect_indexed_y(CPU* cpu) {
    return load_indirect_indexed(cpu, cpu->idx_reg_y);
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

/* TODO (ctfloyd) Check (and increase cycle count) if load crosses page boundary */
static inline int lda_absolute_x(CPU* cpu) {
    Byte accumulator_byte = load_absolute_value(cpu, cpu->idx_reg_x);
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 4;
}

/* TODO (ctfloyd) Check (and increase cycle count) if load crosses page boundary */
static inline int lda_absolute_y(CPU* cpu) {
    Byte accumulator_byte = load_absolute_value(cpu, cpu->idx_reg_y);
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 4;
}

static inline int lda_indirect_x(CPU* cpu) {
    Byte accumulator_byte = load_indexed_indirect_x(cpu);
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 6;
}

/* TODO (ctfloyd) Check (and increase cycle count) if load crosses page boundary */
static inline int lda_indirect_y(CPU* cpu) {
    Byte accumulator_byte = load_indirect_indexed_y(cpu);
    cpu->accumulator = accumulator_byte;
    flags_set_nz(&cpu->flags, accumulator_byte);
    return 5;
}

static inline int ldx_imm(CPU* cpu) {
    Byte x_byte = cpu_load_next_byte(cpu);
    cpu->idx_reg_x = x_byte;
    flags_set_nz(&cpu->flags, x_byte);
    return 2;
}

static inline int ldx_zero(CPU* cpu) {
    Byte x_byte = load_zero_page_value(cpu, 0);
    cpu->idx_reg_x = x_byte;
    flags_set_nz(&cpu->flags, x_byte);
    return 3;
}

static inline int ldx_zero_y(CPU* cpu) {
    Byte x_byte = load_zero_page_value(cpu, cpu->idx_reg_y);
    cpu->idx_reg_x = x_byte;
    flags_set_nz(&cpu->flags, x_byte);
    return 4;
}

static inline int ldx_abs(CPU* cpu) {
    Byte x_byte = load_absolute_value(cpu, 0);
    cpu->idx_reg_x = x_byte;
    flags_set_nz(&cpu->flags, x_byte);
    return 4;
}


/* TODO (ctfloyd): Increase cycle count by one if page boundary is crossed */
static inline int ldx_abs_y(CPU* cpu) {
    Byte x_byte = load_absolute_value(cpu, cpu->idx_reg_y);
    cpu->idx_reg_x = x_byte;
    flags_set_nz(&cpu->flags, x_byte);
    return 4;
}

#endif

