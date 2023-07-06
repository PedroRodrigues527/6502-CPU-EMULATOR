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

    Byte &operator[](u32 addr) // write to memory (1 byte)
    {
        return Data[addr];
    }

    void writeWord(Word value, u32 addr, u32 &clockCycles) // writes 2 bytes
    {
        Data[addr] = value & 0xFF;
        Data[addr + 1] = (value >> 8);

        std::cout << "\nWRITE WORD\n";
        std::cout << value;
        std::cout << (int)Data[addr];
        std::cout << "\n";
        std::cout << (int)Data[addr + 1];
        clockCycles -= 2;
        std::cout << "\n";
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
        std::cout << "fetching byte\n";
        std::cout << "ProgramCounter-> ";
        std::cout << ProgramCounter;
        Byte Data = memory[ProgramCounter];
        std::cout << "\nData-> ";
        std::cout << "";
        std::cout << (int)Data;
        std::cout << "\n";

        ProgramCounter++;
        ClockCycles--;
        return Data;
    }

    Word fetchWord(u32 &ClockCycles, Memory &memory)
    {
        Word Data = memory[ProgramCounter];
        ProgramCounter++;
        ClockCycles--;

        Data |= (memory[ProgramCounter] << 8);
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

    void LOAD_flag_processing(Byte value)
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
        std::cout << "**** start exec ****\n";
        while (ClockCycles > 0)
        {
            std::cout << "starting fetch: \n";
            Byte instruction = fetchByte(ClockCycles, memory);
            std::cout << "\ninstruction: ";
            std::cout << (int)instruction;
            std::cout << "\n";

            switch (instruction)
            {
            case opcodes::LDA:
            {
                Byte value = fetchByte(ClockCycles, memory);
                Acc = value;
                LOAD_flag_processing(value);
                std::cout << "lda\n";
                std::cout << "value->";
                std::cout << (int)value;
                std::cout << "\nprocessor status->";
                std::cout << ProcessorStatus;
                std::cout << "\n";
                break;
            }
            case opcodes::LDA_ZERO_PAGE:
            {
                Byte address = fetchByte(ClockCycles, memory);
                Acc = readByte(ClockCycles, memory, address);
                LOAD_flag_processing(Acc);
                break;
            }
            case opcodes::LDA_ZERO_PAGE_X:
            {
                Byte address = fetchByte(ClockCycles, memory);
                address += RX;
                ClockCycles--;
                // TODO: verify when address overflows
                Acc = readByte(ClockCycles, memory, address);
                LOAD_flag_processing(Acc);
                break;
            }
            case opcodes::LDX:
            {
                Byte value = fetchByte(ClockCycles, memory);
                RX = value;
                LOAD_flag_processing(value);
                break;
            }
            case opcodes::LDX_ZERO_PAGE:
            {
                Byte address = fetchByte(ClockCycles, memory);
                RX = readByte(ClockCycles, memory, address);
                LOAD_flag_processing(RX);
                break;
            }
            case opcodes::LDY:
            {
                Byte value = fetchByte(ClockCycles, memory);
                RY = value;
                LOAD_flag_processing(value);
                break;
            }
            case opcodes::LDY_ZERO_PAGE:
            {
                Byte address = fetchByte(ClockCycles, memory);
                RY = readByte(ClockCycles, memory, address);
                LOAD_flag_processing(RX);
                break;
            }
            case opcodes::JSR:
            {
                Word sub_routine_address = fetchWord(ClockCycles, memory);
                memory.writeWord(ProgramCounter - 1, StackPointer++, ClockCycles);
                ProgramCounter = sub_routine_address;
                ClockCycles--;
                break;
            }
            default:
                std::cout << "COMMAND NOT FOUND";
                break;
            }
        }
    }
};

void loadTestProgram(Memory &memory)
{
    memory[0xfffC] = opcodes::JSR;
    memory[0xfffD] = 0x42;
    memory[0xfffe] = 0x42;
    memory[0x4242] = opcodes::LDA;
    memory[0x4243] = 0x84;
}

int main()
{
    Memory memory;
    CPU cpu;
    cpu.reset(memory);
    loadTestProgram(memory);
    cpu.exec(9, memory);
    return 0;
};