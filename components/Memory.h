#ifndef MEMORY_H
#define MEMORY_H
#include "../data_types/data_types.h"
#include <cstdint>

struct Memory
{
    static constexpr u32 MEMORY_MAX_SIZE = 1024 * 64; // 64KBytes
    Byte Data[MEMORY_MAX_SIZE];

    void init();

    Byte operator[](u32 addr) const;

    Byte &operator[](u32 addr);

    void writeWord(Word value, u32 addr, u32 &clockCycles);
};

#endif
