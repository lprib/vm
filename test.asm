:loop
load var_i
inc

loadimm var_startstr
//PrintString
io 0

//print cube
dup
dup
dup
umul
umul

//PrintDecimal
io 1

loadimm var_endstr
//PrintString
io 0

@store var_i

load var_iters
jumpneq loop


halt
// vars
:var_i
#words 0
:var_iters
#words 10

:var_startstr
#string Cubed value -->
#words 0

:var_endstr
#string <--\n
#words 0
