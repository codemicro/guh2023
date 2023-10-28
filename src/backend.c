//
// Created by akp on 28/10/23.
//

#if defined(BACKEND_CALC)
#include "backends/calc.c"
#elif defined(BACKEND_PC)
#include "backends/pc.c"
#else
#error "Missing backend definition"
#endif
