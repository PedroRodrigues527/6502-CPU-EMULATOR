#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using Byte = unsigned char;  // 1 byte (8 bits)
using Word = unsigned short; // 2 bytes (16 bits)

struct Memory
{
    static constexpr unsigned int MEMORY_MAX_SIZE = 1024 * 64; // 64KBytes
    Byte Data[MEMORY_MAX_SIZE];

    void init()
    {
        // reset memory
        for (unsigned int i = 0; i < MEMORY_MAX_SIZE; i++)
        {
            Data[i] = 0;
        }
    }
};

struct CPU
{
    Word ProgramCounter;
    Word StackPointer; // supports a 256 byte stack located between $0100 and $01FF

    // registers
    Byte Acc; // supports a 256 byte stack located between $0100 and $01FF
    Byte RX, RY;

    // flags
    Byte C : 1; // carry
    Byte Z : 1; // zero
    Byte I : 1; // interrupt
    Byte D : 1; // decimal
    Byte B : 1; // break
    Byte O : 1; // overflow
    Byte N : 1; // negative

    void reset(Memory memory)
    {
        I = 1;
        ProgramCounter = 0xFFFc;
        StackPointer = 0x0100;
        D = 0; // clear decimal flag
        I = 0; // clear interrupt flag
        Z = D = B = O = N = 0;
        // registers to 0?
        Acc = RX = RY = 0;

        memory.init();
    }
};

int main()
{
    Memory memory;
    CPU cpu;
    cpu.reset(memory);
    return 0;
}