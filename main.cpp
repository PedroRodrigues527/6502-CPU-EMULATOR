#include <iostream>
#include <stdio.h>
#include <stdlib.h>
/* https://www.youtube.com/watch?v=qJgsuQoy9bc */
struct CPU
{
    using Byte = unsigned char;  // 1 byte (8 bits)
    using Word = unsigned short; // 2 bytes (16 bits)

    Word ProgramCounter;
    //TODO: if problem replace with Word
    Byte StackPointer; // supports a 256 byte stack located between $0100 and $01FF

    //registers
    Byte Accumulator; // supports a 256 byte stack located between $0100 and $01FF
    Byte RegisterX, RegisterY;

    Byte ProcessorStatus;
};

int main()
{
    std::cout << "Hello";

    return 0;
}