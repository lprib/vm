#include "vm_Opcodes.h"

#include <stdio.h>

/** Ouptputs opcode schema in CSV form to std output. This allows schema used
 * in assembler to evolve with the code */
int main(void)
{
#define X(name, numInlineArgs, _operation) \
    printf(#name "," #numInlineArgs "\n");

    VM_ALL_OPCODES

#undef X
}
