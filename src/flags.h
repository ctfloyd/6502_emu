#include "types.h"
#include <stdbool.h>

#ifndef FLAGS_H
#define FLAGS_H

typedef struct Flags {
	bool carry: 1;
	bool zero: 1;
	bool interrupt_disable: 1;
	bool decimal_mode: 1;
	bool break_command: 1;
	bool overflow: 1;
	bool negative: 1;
} Flags;

static inline void flags_set_nz(Flags* flags, Byte accumulator_byte) {
    if(accumulator_byte == 0) {
        flags->zero = true;
    } else {
        flags->zero = false;
    }

    if((s8)accumulator_byte < 0) {
        flags->negative = true;
    } else {
        flags->negative = false;
    }
}

#endif