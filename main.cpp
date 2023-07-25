#include "opcodes/opcodes.h"
#include "cycles/cycles.h"
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

        clockCycles -= 2;
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
        {
            ProcessorStatus.set(NEGATIVE_FLAG, 1);
        }
    }

    void exec(u32 ClockCycles, Memory &memory)
    {
        while (ClockCycles > 0)
        {
            Byte instruction = fetchByte(ClockCycles, memory);

            switch (instruction)
            {
            case opcodes::LDA:
            {
                Byte value = fetchByte(ClockCycles, memory);
                Acc = value;
                LOAD_flag_processing(value);
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
            case opcodes::STA_ZERO_PAGE:
            {
                Byte address = fetchByte(ClockCycles, memory);
                /* std::cout << "address" << (int) address; */
                Byte address_value = readByte(ClockCycles, memory, address);
                Acc = address_value;
                /* ClockCycles--; */

                break;
            }
            default:
                std::cout << "COMMAND NOT FOUND: ";
                std::cout << (int)instruction;
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

bool isCPUWithErrors(Memory &memory, CPU &cpu)
{
    std::cout << "TESTING CPU...\n";

    // LDA
    memory[0xfffC] = opcodes::LDA;
    memory[0xfffD] = 0x42;

    cpu.exec(cycles::LOAD_CYCLES, memory);

    if (cpu.Acc != 0x42)
    {
        std::cout << "LDA ERROR";
        return 1;
    }

    cpu.reset(memory);

    // LDA ZERO PAGE
    memory[0xfffC] = opcodes::LDA_ZERO_PAGE;
    memory[0xfffD] = 0xff;
    memory[0xff] = 0x42;

    cpu.exec(cycles::LOAD_ZERO_CYCLES, memory);

    if (cpu.Acc != 0x42)
    {
        std::cout << "LDA ZERO PAGE ERROR";
        return 1;
    }

    cpu.reset(memory);

    // LDA ZERO PAGE X
    memory[0xfffC] = opcodes::LDA_ZERO_PAGE_X;
    cpu.RX = 0x11;
    memory[0xfffD] = 0x20;

    memory[0x31] = 0x22;

    cpu.exec(cycles::LOAD_ZERO_X_CYCLES, memory);

    if (cpu.Acc != 0x22)
    {
        std::cout << "\nLDA ZERO PAGE X ERROR\n";
        return 1;
    }

    cpu.reset(memory);

    // LDX
    memory[0xfffC] = opcodes::LDX;
    memory[0xfffD] = 0x42;

    cpu.exec(cycles::LOAD_CYCLES, memory);

    if (cpu.RX != 0x42)
    {
        std::cout << "LDX ERROR";
        return 1;
    }

    cpu.reset(memory);

    // LDX ZERO PAGE
    memory[0xfffC] = opcodes::LDX_ZERO_PAGE;
    memory[0xfffD] = 0xff;
    memory[0xff] = 0x42;

    cpu.exec(cycles::LOAD_ZERO_CYCLES, memory);

    if (cpu.RX != 0x42)
    {
        std::cout << "LDX ZERO PAGE ERROR";
        return 1;
    }

    cpu.reset(memory);

    // LDY
    memory[0xfffC] = opcodes::LDY;
    memory[0xfffD] = 0x42;

    cpu.exec(cycles::LOAD_CYCLES, memory);

    if (cpu.RY != 0x42)
    {
        std::cout << "LDY ERROR";
        return 1;
    }

    cpu.reset(memory);

    // LDY ZERO PAGE
    memory[0xfffC] = opcodes::LDY_ZERO_PAGE;
    memory[0xfffD] = 0xff;
    memory[0xff] = 0x42;

    cpu.exec(cycles::LOAD_ZERO_CYCLES, memory);

    if (cpu.RY != 0x42)
    {
        std::cout << "LDY ZERO PAGE ERROR";
        return 1;
    }

    cpu.reset(memory);

    // JSR
    memory[0xfffC] = opcodes::JSR;
    memory[0xfffD] = 0x42;
    memory[0xfffe] = 0x42;
    memory[0x4242] = opcodes::LDA;
    memory[0x4243] = 0x84;

    cpu.exec(cycles::JSR_CYCLES, memory);

    if (cpu.Acc != 0x84)
    {
        std::cout << "JSR ERROR";
        return 1;
    }

    cpu.reset(memory);

    // STA ZERO PAGE
    memory[0xfffC] = opcodes::STA_ZERO_PAGE;
    memory[0xfffD] = 0xff;
    memory[0xff] = 0x41;

    cpu.exec(cycles::STA_ZERO_PAGE, memory);

    if (cpu.Acc != 0x41)
    {
        std::cout << "STA ERROR";
        return 1;
    }

    std::cout << "\nALL TESTS PASSED!\n";
    return 0;
}

int main(int argc, char *argv[])
{
    Memory memory;
    CPU cpu;
    cpu.reset(memory);

    if (argc >= 2 && std::string(argv[1]) == "--check_errors" && isCPUWithErrors(memory, cpu))
    {
        return EXIT_FAILURE;
    }
    else
    {
        cpu.reset(memory);
        loadTestProgram(memory);
        cpu.exec(cycles::JSR_CYCLES, memory); // verify number of cycles
        return 0;
    }
    return 0;
};