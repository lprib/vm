:loop
load var_i
inc

loadimm var_startstr
io PrintString

//print cube
dup
dup
dup
umul
umul

io PrintDecimal

loadimm var_endstr
io PrintString

@store var_i

load var_iters
jumpneq loop


halt
// vars
:var_i
#words 0
:var_iters
#words 0xf

:var_startstr
#string Cubed value -->
#words 0

:var_endstr
#string <--\n
#words 0
