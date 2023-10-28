//
// Created by akp on 28/10/23.
//

#include "backends/calc.c"

#if defined(BACKEND_CALC)
int main()
#elif defined(BACKEND_PC)
int main(int argc, char** argv)
#endif
{
    initDisplay();
    while (1) {
    }
    return 0;
}
