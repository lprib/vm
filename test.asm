:loop
LOAD i
INC

//print square
dup
dup
umul
IO 0

@STORE i

LOAD iters
JUMPNEQ loop

HALT
// vars
:i
#words 0
:iters
#words 10
