#include "../deps/bdd-for-c.h"
#include "../src/cpu.c"
#include "../src/instruction.h"
#include "stdbool.h"

#define IMM_FLAGS_CHECK(val) {                      \
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

    struct CPU* cpu;
    static int MEMORY_SIZE_IN_BYTES = 32;


    describe("reset") {
        before_each() {
            cpu = cpu_create(MEMORY_SIZE_IN_BYTES);
        }

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
            for(u8 i = 0; i < MEMORY_SIZE_IN_BYTES; i++) {
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
                    IMM_FLAGS_CHECK(POS_SENTINEL);
                }

                it("should set flags correctly for negative imm value") {
                    cpu->memory.data[1] = NEG_SENTINEL;
                    IMM_FLAGS_CHECK(NEG_SENTINEL);
                }

                it("should set flags correctly for zero imm value") {
                    cpu->memory.data[1] = 0;
                    IMM_FLAGS_CHECK(0);
                }

                it("should take two cpu cycles to run") {
                    int cycles = cpu_run(cpu, 10);
                    check(cycles == 2);
                }
            }
        }
    }
}
