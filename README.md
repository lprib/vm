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
- [ ] Set up hello world build system
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

### IO
IO with a register with instruction. Used for platform specifics, but with unified API

### Arrays

