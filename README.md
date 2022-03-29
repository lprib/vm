# VM2

## Requirements
1. Executable bitcode from buffer or comm bridge
2. Simple ISA
3. FPGA implementable
4. High level as possible without complex implementation
5. Easy IO
6. Extendable

## Specifications per Reqs
- (1): no reliance on platform in codebase. Ie no use of peripherals except for generic IO
- (5, 6): Unified IO for performing platform calls as well as IO.
	- Same interface for function calls as IO?
- (3): No looking up functions by name in bitcode (as in JVM)
- (6): Able to add own functions per platform

## Roadmap
- [x] Set up hello world build system
- [x] Set up per-module unit testing
- [ ] ISA investigation
- [ ] ISA implementation

## ISA notes
16bit operation
stack based (easier compilation in future)
also includes program memory with load/store

Loop:
	- fetch opcode
	- fetch args (if any)
	- exe opcode
	- update PC


Provide stack overflow and underflow hooks

## ISA implementation
- `LOAD [n]`: push value at addr n to stack
- `STORE [n]`: pop stack and save at addr n
- `LOAD_IMM [n]`: push value n to stack
- `PICK [n]`: pick the nth top most value from stack, and duplicate it on to the stack
- `TAKE [n]`: take the nth top most value from stack, move it to the top of the
  stack. All other values are shuffled accordingly.
- `DUP`: duplicate top item on stack
- `SWAP`: swap top 2 items on stack
- `DROP`: remove top element from stack
- `IO [n]`: Perform IO call to function at index n, see Platform IO Interface
- `HALT`: Halts the VM. `vm_ProcessOpcode` will return
  `VM_PROCESS_PROGRAM_HALT` instead of a continue result.
- `JUMP [n]`: unconditional branch to addr n
- `JUMP(EQ|NEQ|LT|GT|LEQ|GEQ) [n]`: conditional branch to n based on top 2 items on stack
- Binary ops take 2 items from stack and push result to stack:
	- `ADD`: addition
	- `USUB`: unsigned subtract
	- `SSUB`: signed subtract
	- `UMUL`: unsigned multiply
	- `SMUL`: signed multiply
	- `DIV`: unsigned division
	- `AND`: bitwise and 
	- `OR`: bitwise or
	- `XOR`: bitwise xor
	- `MOD`: modulo
- Unary ops take 1 item from stack and push result to stack:
	- `INC`: increment
	- `DEC`: decrement
	- `INV`: bitwise invert

Each opcode can be ORed with `VM_PEEK_BITMASK`, which will make it peek its
arguments from the stack instead of popping them. Helps with using the stack as
more of an extended register set. This also applies to IO function calls, which
will peek all of their arguments if the bit is set.

## Platform IO Interface
IO functions are implmented by the client of this API, and can include
arguments and a return, all `vm_uints`. Argments are popped (or peeked if peek
bit set) from the stack. Optional return is pushed to the stack.

If the function number requested by the bytecode is not an implemented function
by the platform, `vm_ProcessOpcode` will return `VM_PROCESS_ERROR_UNDEF_IO_FN`.

- Include `vm_PlatformInterface.h`.
- Provide your own `vmint_IoFunctionRegistry.h"
	- define `VM_IO_FN_REGISTRY` with X macros
	- `X(name, numArgs, hasReturn)`
	- `X(io_MyFunction, 2, false)`
- `VM_DEFINE_IO_INTERFACE(name)`
	- use input pointer to get args, output return to set return
	- inputs come from stack, output goes to stack
- TODO: implement IO function which delegates
- TODO: Abstract notion of "next opcode" which can be from an array or stream
	- WONT DO: need to support jumps

# To add an opcode
- Add to vm_Opcodes.h
- Handle in vm_ProcessOpcode.c
- Add unit test in test_vm_ProcessOpcode.c
- Document in README.md
