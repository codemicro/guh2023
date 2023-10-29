#define BACKEND_CALC 1

#if defined(BACKEND_CALC)
#include "backends/calc.h"
#elif defined(BACKEND_PC)
#include "backends/pc.h"
#else
#error "Missing backend definition"
#endif