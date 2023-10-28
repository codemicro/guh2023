#define BACKEND_PC 1

#if defined(BACKEND_PC)
#include "backends/pc.h"
#elif defined(BACKEND_CALC)
#include "backends/calc.h"
#else
#error "Missing backend definition"
#endif