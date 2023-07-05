#include "opcodes/opcodes.h"
#include "data_types/data_types.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>

// 6502 documentation
// https://web.archive.org/web/20190130171422/http://www.obelisk.me.uk/6502/

#define CARRY_FLAG 0
#define ZERO_FLAG 1
#define INTERRUPT_DISABLE 2
#define DECIMAL_MODE_FLAG 3
#define BREAK_COMMAND 4
#define OVERFLOW_FLAG 5
#define NEGATIVE_FLAG 6

struct Memory
{
    static constexpr u32 MEMORY_MAX_SIZE = 1024 * 64; // 64KBytes
    Byte Data[MEMORY_MAX_SIZE];

    void init()
    {
        // reset memory
        for (u32 i = 0; i < MEMORY_MAX_SIZE; i++)
        {
            Data[i] = 0;
        }
    }

    Byte operator[](u32 addr) const
    {
        return Data[addr];
    }

    Byte &operator[](u32 addr) // write to memory
    {
        return Data[addr];
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
    std::bitset<7> ProcessorStatus;

    void reset(Memory &memory)
    {
        ProgramCounter = 0xFFFc;
        StackPointer = 0x0100;
        // ProcessorStatus is initialized as [0,0,0,0,0,0,0,0]
        Acc = RX = RY = 0;

        memory.init();
    }

    Byte fetchByte(u32 &ClockCycles, Memory &memory)
    {
        Byte Data = memory[ProgramCounter];
        ProgramCounter++;
        ClockCycles--;
        return Data;
    }

    Byte readByte(u32 &ClockCycles, Memory &memory, Byte address)
    {
        Byte Data = memory[address];
        ClockCycles--;
        return Data;
    }

    void LDA_flag_processing(Byte value)
    {
        if (value == 0)
        {
            ProcessorStatus.set(ZERO_FLAG, 1);
        }
        if (value & (1 << 7)) // if last bit is set
        // or (value & 0b10000000) > 0
        {
            ProcessorStatus.set(NEGATIVE_FLAG, 1);
        }
    }

    void exec(u32 ClockCycles, Memory &memory)
    {
        while (ClockCycles > 0)
        {
            Byte instruction = fetchByte(ClockCycles, memory); // loads number 66 to memory

            switch (instruction)
            {
            case opcodes::LDA:
            {
                Byte value = fetchByte(ClockCycles, memory);
                Acc = value;
                LDA_flag_processing(value);
                /* std::cout << "lda\n";
                std::cout << "value->";
                std::cout << (int)value;
                std::cout << "\nprocessor status->";
                std::cout << ProcessorStatus;
                std::cout << "\n"; */
            }
            case opcodes::LDA_ZERO_PAGE:
            {
                Byte address = fetchByte(ClockCycles, memory);
                Acc = readByte(ClockCycles, memory, address);
                LDA_flag_processing(Acc);
            }
            case opcodes::LDA_ZERO_PAGE_X:
            {
                Byte address = fetchByte(ClockCycles, memory);
                address += RX;
                ClockCycles--;
                // TODO: verify when address overflows
                Acc = readByte(ClockCycles, memory, address);
                LDA_flag_processing(Acc);
            }
            break;
            default:
                std::cout << "COMMAND NOT FOUND";

                break;
            }
        }
    }
};

void loadProgram(Memory &memory)
{
    memory[0xfffC] = opcodes::LDA_ZERO_PAGE;
    memory[0xfffD] = 0x42;
    memory[0x42] = 0x84; // loads number 132 to memory
}

int main()
{
    std::cout << "\n";
    Memory memory;
    CPU cpu;
    cpu.reset(memory);
    loadProgram(memory);
    cpu.exec(3, memory); // executes 3 instructions from memory
    return 0;
}
