#include "../deps/bdd-for-c.h"
#include "../src/cpu.c"
#include "../src/instruction.h"
#include "stdbool.h"
#include <stdio.h>

#define NZ_FLAGS_CHECK(val) {                      \
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

        before_each() {
            cpu_reset(cpu);
        }

        describe("LDA") {
            describe("IMM") {
                static int POS_SENTINEL = 40;
                static int NEG_SENTINEL = -40;

                before_each() {
                    cpu->memory.data[0] = LDA_IMM;
                }

                it("should load the next byte of memory into the accumulator") {
                    cpu->memory.data[1] = POS_SENTINEL;

                    cpu_run(cpu, 1);
                    check(cpu->accumulator == POS_SENTINEL);
                }

                it("should set flags correctly for positive imm value") {
                    cpu->memory.data[1] = POS_SENTINEL;
                    NZ_FLAGS_CHECK(POS_SENTINEL);
                }

                it("should set flags correctly for negative imm value") {
                    cpu->memory.data[1] = NEG_SENTINEL;
                    NZ_FLAGS_CHECK(NEG_SENTINEL);
                }

                it("should set flags correctly for zero imm value") {
                    cpu->memory.data[1] = 0;
                    NZ_FLAGS_CHECK(0);
                }

                it("should take two cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 2);
                }
            }

            describe("ZERO") {
                static int POS_SENTINEL = 40;
                static int NEG_SENTINEL = -40;

                before_each() {
                    cpu->memory.data[0] = LDA_ZERO;
                    cpu->memory.data[1] = 32;
                }

                it("should load the value at specified address into accumulator") {
                    cpu->memory.data[32] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->accumulator == POS_SENTINEL);
                }

                it("should set flags correctly for positive sentinel") {
                    cpu->memory.data[32] = POS_SENTINEL;
                    NZ_FLAGS_CHECK(POS_SENTINEL);
                }

                it("should set flags correctly for zero sentinel") {
                    cpu->memory.data[32] = 0;
                    NZ_FLAGS_CHECK(0);
                }

                it("should set flags correctly for negative sentinel") {
                    cpu->memory.data[32] = NEG_SENTINEL;
                    NZ_FLAGS_CHECK(NEG_SENTINEL);
                }

                it("should take three cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 3);
                }
            }

            describe("ZERO, X") {
                static int POS_SENTINEL = 40;
                static int NEG_SENTINEL = -40;
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

                it("should set flags correctly for positive sentinel") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    NZ_FLAGS_CHECK(POS_SENTINEL);
                }

                it("should set flags correctly for zero sentinel") {
                    cpu->memory.data[DESTINATION] = 0;
                    NZ_FLAGS_CHECK(0);
                }

                it("should set flags correctly for negative sentinel") {
                    cpu->memory.data[DESTINATION] = NEG_SENTINEL;
                    NZ_FLAGS_CHECK(NEG_SENTINEL);
                }

                it("should wrap around the page boundary") {
                    int FAR_OFFSET = 200;
                    int PAGE_SIZE = 0xFF;
                    cpu->memory.data[1] = FAR_OFFSET;
                    cpu->idx_reg_x = PAGE_SIZE - FAR_OFFSET + DESTINATION + 1;
                    cpu->memory.data[DESTINATION] = NEG_SENTINEL;
                    cpu_run(cpu, 1);
                    check((s8)(cpu->accumulator) == NEG_SENTINEL);
                }

                it("should take four cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 4);
                }
            }

            describe("ABS") {
                static int POS_SENTINEL = 40;
                static int NEG_SENTINEL = -40;
                static int DESTINATION = 1024;

                before_each() {
                    cpu->memory.data[0] = LDA_ABS;

                    // 1024 in little endian
                    cpu->memory.data[1] = 0x00;
                    cpu->memory.data[2] = 0x04;
                }

                it("should load the value at the specified address into accumulator") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->accumulator == POS_SENTINEL);
                }

                it("should set flags correctly for positive sentinel") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    NZ_FLAGS_CHECK(POS_SENTINEL);
                }

                it("should set flags correctly for zero sentinel") {
                    cpu->memory.data[DESTINATION] = 0;
                    NZ_FLAGS_CHECK(0);
                }

                it("should set flags correctly for negative sentinel") {
                    cpu->memory.data[DESTINATION] = NEG_SENTINEL;
                    NZ_FLAGS_CHECK(NEG_SENTINEL);
                }

                it("should take four cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 4);
                }
            }

            describe("ABS, X") {
                static int POS_SENTINEL = 40;
                static int NEG_SENTINEL = -40;
                static int OFFSET = 20; 
                static int DESTINATION = 1024 + 20;

                before_each() {
                    cpu->memory.data[0] = LDA_ABS_X;

                    // 1024 in little endian
                    cpu->memory.data[1] = 0x00;
                    cpu->memory.data[2] = 0x04;

                    cpu->idx_reg_x = OFFSET;
                }

                it("should load the value at the specified address and (x) offset into accumulator") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->accumulator == POS_SENTINEL);
                }

                it("should set flags correctly for positive sentinel") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    NZ_FLAGS_CHECK(POS_SENTINEL);
                }

                it("should set flags correctly for zero sentinel") {
                    cpu->memory.data[DESTINATION] = 0;
                    NZ_FLAGS_CHECK(0);
                }

                it("should set flags correctly for negative sentinel") {
                    cpu->memory.data[DESTINATION] = NEG_SENTINEL;
                    NZ_FLAGS_CHECK(NEG_SENTINEL);
                }

                it("should take four cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 4);
                }
            }

            describe("ABS, Y") {
                static int POS_SENTINEL = 40;
                static int NEG_SENTINEL = -40;
                static int OFFSET = 20; 
                static int DESTINATION = 1024 + 20;

                before_each() {
                    cpu->memory.data[0] = LDA_ABS_Y;

                    // 1024 in little endian
                    cpu->memory.data[1] = 0x00;
                    cpu->memory.data[2] = 0x04;

                    cpu->idx_reg_y = OFFSET;
                }

                it("should load the value at the specified address and (x) offset into accumulator") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    cpu_run(cpu, 1);
                    check(cpu->accumulator == POS_SENTINEL);
                }

                it("should set flags correctly for positive sentinel") {
                    cpu->memory.data[DESTINATION] = POS_SENTINEL;
                    NZ_FLAGS_CHECK(POS_SENTINEL);
                }

                it("should set flags correctly for zero sentinel") {
                    cpu->memory.data[DESTINATION] = 0;
                    NZ_FLAGS_CHECK(0);
                }

                it("should set flags correctly for negative sentinel") {
                    cpu->memory.data[DESTINATION] = NEG_SENTINEL;
                    NZ_FLAGS_CHECK(NEG_SENTINEL);
                }

                it("should take four cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 4);
                }
            }
        }
    }
}
