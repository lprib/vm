// vm_Opcodes.h will be included by preprocessor args, no need to include it
// here. This reduces noise in the preprocessed output.

// This file need not compile, it will only be preprocessed to generate a csv of
// opcode names and arguments.

// Must generate "name1,numArgs;name2,numArgs". All whitespace will be removed
// and ";" replaced by "\n" in the makefile for easy parsing by the assembler.

#define X(name, numInlineArgs, _tokens) name, numInlineArgs;
VM_ALL_OPCODES
#undef X
