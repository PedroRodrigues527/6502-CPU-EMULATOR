#include "Memory.h"

void Memory::init()
{
    for (u32 i = 0; i < MEMORY_MAX_SIZE; i++)
    {
        Data[i] = 0;
    }
}

Byte Memory::operator[](u32 addr) const
{
    return Data[addr];
}

Byte &Memory::operator[](u32 addr)
{
    return Data[addr];
}

void Memory::writeWord(Word value, u32 addr, u32 &clockCycles)
{
    Data[addr] = value & 0xFF;
    Data[addr + 1] = (value >> 8);

    clockCycles -= 2;
}
