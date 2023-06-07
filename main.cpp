#include <iostream>
#include <stdio.h>
#include <stdlib.h>
/* https://www.youtube.com/watch?v=qJgsuQoy9bc */

// processor status
struct PS 
{
    unsigned char : 1;
};

struct CPU
{
    using Byte = unsigned char;  // 1 byte (8 bits)
    using Word = unsigned short; // 2 bytes (16 bits)

    Word ProgramCounter;
    // TODO: if problem replace with Word
    Byte StackPointer; // supports a 256 byte stack located between $0100 and $01FF

    // registers
    Byte Acc; // supports a 256 byte stack located between $0100 and $01FF
    Byte RX, RY;

    // flags
    PS C; // carry
    PS Z; // zero
    PS I; // interrupt
    PS D; // decimal
    PS B; // break
    PS O; // overflow
    PS N; // negative
};

int main()
{
    CPU cpu;
    return 0;
}