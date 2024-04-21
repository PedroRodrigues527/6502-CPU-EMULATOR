#include "Memory.h"

/**
 * Memory initialization
*/
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

/**
 * Write word to a given location
 * @param Word value
 * @param u32 addr
 * @param s32 ClockCycles
*/
void Memory::writeWord(Word value, u32 addr, s32 &clockCycles)
{
    Data[addr] = value & 0xFF;
    Data[addr + 1] = (value >> 8);

    clockCycles -= 2;
}

/**
 * Write byte to a given location
 * @param BYte value
 * @param u32 addr
 * @param s32 ClockCycles
*/
void Memory::writeByte(Byte value, u32 addr, s32 &clockCycles)
{
    Data[addr] = value;
    clockCycles--;
}

