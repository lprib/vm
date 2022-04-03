// vm_Opcodes.h will be included by preprocessor args, no need to include it
// here. This reduces noise in the preprocessed output.

// This file need not compile, it will only be preprocessed to generate a csv of
// opcode names and arguments.

// Must generate "name1,numArgs;name2,numArgs". All whitespace will be removed
// and ";" replaced by "\n" in the makefile for easy parsing by the assembler.

#define X(specialOpcodeName, numInlineArgs) specialOpcodeName, numInlineArgs;
VM_SPECIAL_OPCODES
#undef X
#define X(jumpOpcodeName, comparisionOperation) jumpOpcodeName, 2;
VM_JUMP_OPCODES
#undef X
#define X(binaryOpcodeName, operation) binaryOpcodeName, 2;
VM_UNSIGNED_UNARY_OPCODES
VM_SIGNED_BINARY_OPCODES
#undef X
#define X(unaryOpcodeName, operation) unaryOpcodeName, 1;
VM_UNSIGNED_UNARY_OPCODES
#undef X
