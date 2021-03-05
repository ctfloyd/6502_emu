#include "../deps/bdd-for-c.h"
#include "../src/cpu.c"
#include "../src/instruction.h"
#include "stdbool.h"
#include <stdio.h>

#define NZ_FLAGS_CHECK(val) {                       \
    if (val > 0) {                                  \
        cpu->flags.zero = true;                     \
        cpu->flags.negative = true;                 \
                                                    \
        cpu_run(cpu, 1);                            \
        check(cpu->flags.zero == false);            \
        check(cpu->flags.negative == false);        \
    } else if (val < 0) {                           \
        cpu->flags.zero = true;                     \
        cpu->flags.negative = false;                \
                                                    \
        cpu_run(cpu, 1);                            \
        check(cpu->flags.zero == false);            \
        check(cpu->flags.negative == true);         \
    } else {                                        \
        cpu->flags.zero = false;                    \
        cpu->flags.negative = true;                 \
                                                    \
        cpu_run(cpu, 1);                            \
        check(cpu->flags.zero == true);             \
        check(cpu->flags.negative == false);        \
    } \
}

#define NZ_AUTO_FLAGS_CHECK(dest) {                             \
     it("should set flags correctly for positive imm value") {  \
        cpu->memory.data[dest] = POS_SENTINEL;                  \
        NZ_FLAGS_CHECK(POS_SENTINEL);                           \
    }                                                           \
                                                                \
    it("should set flags correctly for negative imm value") {   \
        cpu->memory.data[dest] = NEG_SENTINEL;                  \
        NZ_FLAGS_CHECK(NEG_SENTINEL);                           \
    }                                                           \
                                                                \
    it("should set flags correctly for zero imm value") {       \
        cpu->memory.data[dest] = 0;                             \
        NZ_FLAGS_CHECK(0);                                      \
    }                                                           \
}


spec("CPU") {

    static CPU* cpu = NULL;
    static int MEMORY_SIZE_IN_BYTES = 8 * 1024;

    before_each() {
        if(cpu != NULL) {
            cpu_destroy(cpu);
        }

        cpu = cpu_create(MEMORY_SIZE_IN_BYTES);
    }

    after() {
        cpu_destroy(cpu);
    }


    describe("reset") {

        it("should contain complete zero'd registers/flags when reset") {
            cpu_reset(cpu);
            check(cpu->accumulator == 0);
            check(cpu->idx_reg_x == 0);
            check(cpu->idx_reg_y == 0);
            check(cpu->program_counter == 0);
            check(cpu->stack_pointer == 0);
            check(cpu->flags.break_command == 0);
            check(cpu->flags.carry == 0);
            check(cpu->flags.decimal_mode == 0);
            check(cpu->flags.interrupt_disable == 0);
            check(cpu->flags.negative == 0);
            check(cpu->flags.overflow == 0);
            check(cpu->flags.zero == 0);
        }

        it("should zero out memory when reset") {
            cpu_reset(cpu);
            for(size_t i = 0; i < MEMORY_SIZE_IN_BYTES; i++) {
                check(cpu->memory.data[i] == 0);
            }
        }
    }

    describe("instructions") {
        static const int POS_SENTINEL = 40;
        static const int NEG_SENTINEL = -40;

        before_each() {
            cpu_reset(cpu);
        }

        describe("LDA") {
            describe("IMM") {

                before_each() {
                    cpu->memory.data[0] = LDA_IMM;
                }

                it("should load the next byte of memory into the accumulator") {
                    cpu->memory.data[1] = POS_SENTINEL;

                    cpu_run(cpu, 1);
                    check(cpu->accumulator == POS_SENTINEL);
                }

                it("should take two cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 2);
                }

                NZ_AUTO_FLAGS_CHECK(1);
            }

            describe("ZERO") {
                before_each() {
                    cpu->memory.data[0] = LDA_ZERO;
                    cpu->memory.data[1] = 32;
                }

                it("should load the value at specified address into accumulator") {
                    cpu->memory.data[32] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->accumulator == POS_SENTINEL);
                }

                it("should take three cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 3);
                }

                NZ_AUTO_FLAGS_CHECK(32);
            }

            describe("ZERO, X") {
                static int DESTINATION = 128;

                before_each() {
                    cpu->memory.data[0] = LDA_ZERO_X;
                    cpu->memory.data[1] = 96;
                    cpu->idx_reg_x = 32;
                }

                it("should load the value at the specified address and (x) offset into accumulator") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->accumulator == POS_SENTINEL);
                }

                it("should take four cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 4);
                }
            
                NZ_AUTO_FLAGS_CHECK(DESTINATION);
            }

            describe("ABS") {
                static int DESTINATION = 1024;

                before_each() {
                    cpu->memory.data[0] = LDA_ABS;

                    // 1024 in little endian
                    cpu->memory.data[1] = 0x00;
                    cpu->memory.data[2] = 0x04;
                }

                it("should take four cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 4);
                }

                NZ_AUTO_FLAGS_CHECK(DESTINATION);
            }

            describe("ABS, X") {
                static int OFFSET = 20; 
                static int DESTINATION = 1024 + 20;

                before_each() {
                    cpu->memory.data[0] = LDA_ABS_X;

                    // 1024 in little endian
                    cpu->memory.data[1] = 0x00;
                    cpu->memory.data[2] = 0x04;

                    cpu->idx_reg_x = OFFSET;
                }

                it("should take four cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 4);
                }

                NZ_AUTO_FLAGS_CHECK(DESTINATION);
            }

            describe("ABS, Y") {
                static int OFFSET = 20; 
                static int DESTINATION = 1024 + 20;

                before_each() {
                    cpu->memory.data[0] = LDA_ABS_Y;

                    // 1024 in little endian
                    cpu->memory.data[1] = 0x00;
                    cpu->memory.data[2] = 0x04;

                    cpu->idx_reg_y = OFFSET;
                }

                it("should load the value at the specified address and (y) offset into accumulator") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->accumulator == POS_SENTINEL);
                }

                it("should take four cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 4);
                }

                NZ_AUTO_FLAGS_CHECK(DESTINATION);
            }

            describe("IND, X") {
                static int OFFSET = 40; 
                static int DESTINATION = 128;
                static int TABLE_PTR = 24;

                before_each() {
                    cpu->memory.data[0] = LDA_IND_X;
                    cpu->memory.data[1] = TABLE_PTR;
                    cpu->idx_reg_x = OFFSET;
                    cpu->memory.data[TABLE_PTR + OFFSET] = DESTINATION;
                }

                it("should load the value at the specified indirect address into accumulator") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->accumulator == POS_SENTINEL);
                }

                it("should take six cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 6);
                }

                NZ_AUTO_FLAGS_CHECK(DESTINATION);
            }

            describe("IND, Y") {
                static const int OFFSET = 40; 
                static const int TABLE_PTR = 24;
                static const int DESTINATION = 128;

                before_each() {
                    cpu->memory.data[0] = LDA_IND_Y;
                    cpu->memory.data[1] = TABLE_PTR;
                    cpu->memory.data[TABLE_PTR] = DESTINATION - OFFSET;
                    cpu->idx_reg_y = OFFSET;
                }

                it("should load the value at the specified indirect address into accumulator") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->accumulator == POS_SENTINEL);
                }

                it("should take six cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 5);
                }

                NZ_AUTO_FLAGS_CHECK(DESTINATION);
            }
        }

        describe("LDX") {
            describe("IMM") {

                before_each() {
                    cpu->memory.data[0] = LDX_IMM;
                }

                it("should load the next byte of memory into the x register") {
                    cpu->memory.data[1] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->idx_reg_x == POS_SENTINEL);
                }

                it("should take two cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 2);
                }

                NZ_AUTO_FLAGS_CHECK(1);
            }

            describe("ZERO") {
                before_each() {
                    cpu->memory.data[0] = LDX_ZERO;
                    cpu->memory.data[1] = 32;
                }

                it("should load the value at specified address into x register") {
                    cpu->memory.data[32] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->idx_reg_x == POS_SENTINEL);
                }

                it("should take three cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 3);
                }

                NZ_AUTO_FLAGS_CHECK(32);
            }

            describe("ZERO, Y") {
                static int DESTINATION = 128;

                before_each() {
                    cpu->memory.data[0] = LDX_ZERO_Y;
                    cpu->memory.data[1] = 96;
                    cpu->idx_reg_y = 32;
                }

                it("should load the value at the specified address and (y) offset into x") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->idx_reg_x == POS_SENTINEL);
                }

                it("should take four cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 4);
                }
            
                NZ_AUTO_FLAGS_CHECK(DESTINATION);
            }

            describe("ABS") {
                static int DESTINATION = 2048;

                before_each() {
                    cpu->memory.data[0] = LDX_ABS;

                    cpu->memory.data[1] = DESTINATION;
                    cpu->memory.data[2] = DESTINATION >> 8;
                }
                
                it("should load the value at the specified address into x") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->idx_reg_x == POS_SENTINEL);
                }

                it("should take four cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 4);
                }

                NZ_AUTO_FLAGS_CHECK(DESTINATION);
            }

            describe("ABS, Y") {
                static int DESTINATION = 2048;
                static u8 OFFSET = 32;

                before_each() {
                    cpu->memory.data[0] = LDX_ABS_Y;

                    cpu->memory.data[1] = DESTINATION;
                    cpu->memory.data[2] = DESTINATION >> 8;

                    cpu->idx_reg_y = OFFSET;
                }

                it("should load the value at the specified address and offset into x") {
                    cpu->memory.data[DESTINATION + OFFSET] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->idx_reg_x == POS_SENTINEL);
                }

                it("should take four cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 4);
                }

                NZ_AUTO_FLAGS_CHECK(DESTINATION + OFFSET);
            }
        }
    }
}
