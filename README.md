# VM2

## Requirements
1. Executable bitcode from buffer or comm bridge
2. Simple ISA
3. FPGA implementable
4. High level as possible without complex implementation
5. Easy IO
6. Extendable

## Specifications per Reqs
- (1): no reliance on platform in codebase. Ie no use of peripherals except for
  generic IO
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

## TODOs
- [x] Make schema generation an executable which prints to stdout (less hacky)
- [ ] Call and ret instructions
- [ ] IO funciton schema from which to generate a ioFunctionName ->
  ioFunctionIndex map in assembler
- [ ] Make vmint_GetIoFunctionRegistry a downward call instead of a link-time
  callback. Should be associated with a certain state.
- [ ] Add const everywhere
- [ ] Add restrict in vm_InitState
- [ ] ambigous whether mem_size is in bytes, words, or vm_uint (should be vm_uint). Document.
- [ ] Finish impl/linux
- [ ] Sort out endianness things (#define specifiation)

## Project structure
- `src/` - Source headers and code (TODO separate public headers to `inc/`)
- `test/` - Unit test sources. To be found by a makefile, must be named same as
  UUT module, with `test_` prefix. Makefile will automatically grab the correct
  UUT module for each test.
- `schema/` - Contains the opcode schema generation header. This is not
  executable and cannot be compiled (only preprocessed).
- `impl/` - Implementation directories.
	- `integration_test/` - Source for integration test implementation.
	- `linux/` - Linux bytecode interpreter implementation (TODO).
- `out/` - Build output files
	- `opcode_schema.csv` - Generated schema file.
	- `*.o` - Intermediate compilation objects. These are shared between
	  test, coverage, and integration compilations, so `make clean` between
	  each compilation type.
	- `impl_exe/` - Output executable for each implementation.
	- `test_exe/` - Output executables for unit tests.

## Makefile
- `make` runs all tests (supress build output)
- `make test` run all tests
- `make integration` build and run impl/integration_test
- `make coverage` build and run tests, and open coverage report (clean before
  this so that it can be recompiled with coverage flags)
- `make schema` generate `out/opcode_schema.csv` which contains names and
  number of args for all recognized opcodes.

### Recursive makefiles
- Each directory in `impl/` has it's own makefile, which is invoked by
  the master with `make impl/directory`
- Recursive makefile will get passed the following variables:
	- `SUBMAKE_OUT`: output directory to put results. This is created by
	  the master makefile before invoking the sub-makefile. It takes the
	  value `./out/impl/directory`
	- `VM_SRC_DIR`: source directory of VM, relative to the sub-makefile
	- `VM_CFLAGS`: flags which are used in the main makefile to build unit
	  tests (don't necessarily need to be used by sub-makefiles.
	- `Q`: set to value "`@`" if quite output is desired, otherwise set to nothing

## ISA implementation
`[...]` denotes a param that is baked in to program memory. `(...)` denotes a
param that is taken from the stack. They are recorded in the order that they
were pushed to stack, ie. `(a) (b)` means b is at the top of the stack.

- `LOAD [addr]`: push value at `addr` in mem to stack
- `STORE [addr] (n)`: store `n` to memory at `addr`
- `LOAD_IMM [val]`: push value n to stack
- `DEREF (addr)`: get memory at addr. Push result to stack.
- `ARRAY_LOAD [base][offset&size] (index)`: Load a value from an array of
  structs located in mem at `base`. Index is taken from stack. The high half of
  `offset&size` (`offset`) determines the offset into the struct (0 if flat
  array). Low half (`size`) sets the size of each member of the array. Setting
  to zero will always grab first member of array regardless of index.
	- `stack <- mem[base + index*size + offset]`
	- Peek bitmask affects this normally, leaving index on the stack underneath
	  the loaded value.
- `ARRAY_STORE [base][offset&size] (value) (index)`: Store a value from the
  stack to an array located in mem at `base`. Same addressing as previous
  opcode.
	- `mem[base + index*size + offset] <- value`
	- Peek bitmask doesn't pop index or value as expected
- `PICK [n]`: remove the nth value on the stack, shuffle items on top to fill
  the gap, and push it to the top. Works with peek bitmask.
- `DUP (val)`: duplicate val and push to stack.
- `SWAP (val1) (val2)`: swap `val1` and `val2` on stack.
- `DROP (val)`: remove val from stack
- `IO [n] (params...)`: Perform IO call to function at index n, see Platform IO Interface
- `HALT`: Halts the VM. `vm_ProcessOpcode` will return
  `VM_PROCESS_PROGRAM_HALT` instead of a continue result.
- `JUMP [n]`: unconditional branch to addr n
- `JUMP(EQ|NEQ|LT|GT|LEQ|GEQ) [n]`: conditional branch to n based on top 2 items on stack
- Binary ops take 2 items from stack and push result to stack (`OPERATION (left) (right)`)
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
	- `SHL`: left shift
	- `LSHR`: logical right shift
	- `ASHR`: arithmetic right shift
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

## To add an opcode
- Add to vm_Opcodes.h
	- Do _not_ add directly to enum, only to relevant X-macros list
- Handle in vm_ProcessOpcode.c
- Add unit test in test_vm_ProcessOpcode.c
- Document in README.md
- Don't worry about assembler, should parse schema generated from vm_OpcodeSchema.h
