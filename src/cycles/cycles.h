#ifndef CYCLES_H
#define CYCLES_H

namespace cycles
{
    extern const int LOAD_CYCLES;
    extern const int LOAD_ZERO_CYCLES; 
    extern const int LOAD_ZERO_X_CYCLES; 

    extern const int JSR_CYCLES; 

    extern const int STA_ZERO_PAGE;
    extern const int STA_ZERO_PAGE_X;
    extern const int STA_ZERO_PAGE_ABSOLUTE;
    extern const int STA_ZERO_PAGE_ABSOLUTE_X;
    extern const int STA_ZERO_PAGE_ABSOLUTE_Y;

    extern const int STX_ZERO_PAGE;
    extern const int STX_ZERO_PAGE_Y;
    extern const int STX_ABSOLUTE;

    extern const int STY_ZERO_PAGE;
    extern const int STY_ZERO_PAGE_X;
    extern const int STY_ABSOLUTE;
       
    
}

#endif