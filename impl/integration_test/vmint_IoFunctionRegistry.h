#ifndef VMINT_IO_FUNCTION_REGISTRY_H
#define VMINT_IO_FUNCTION_REGISTRY_H

#include <stdbool.h>

#define IO_FN_REGISTRY \
    X(it_Return1, 0, true) \
    X(it_Arg1Return1, 1, true) \
    X(it_Arg3Return0, 3, false) \
    X(it_Print, 1, false)

#endif
