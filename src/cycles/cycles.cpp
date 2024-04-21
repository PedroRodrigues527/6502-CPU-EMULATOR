#include "cycles.h"

namespace cycles
{
    const int LOAD_CYCLES = 2; // immediate mode
    const int LOAD_ZERO_CYCLES = 3; 
    const int LOAD_ZERO_X_CYCLES = 4; 
    const int JSR_CYCLES = 8; 
    const int STA_ZERO_PAGE = 3;
    const int STA_ZERO_PAGE_X = 4;
    const int STA_ZERO_PAGE_ABSOLUTE = 4;
    const int STX_ZERO_PAGE = 4;
    const int STX_ZERO_PAGE_Y = 4;
    const int STX_ABSOLUTE = 4;
}