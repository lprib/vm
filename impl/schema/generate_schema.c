#include "opcodes.h"

#include <stdio.h>

/** Ouptputs opcode schema in CSV form to std output. This allows schema used
 * in assembler to evolve with the code */
#define GEN_OPCODE_PRINTER(name, numInlineArgs, _operation) \
    printf(#name "," #numInlineArgs "\n");

int main(void) { VM_ALL_OPCODES(GEN_OPCODE_PRINTER) }
