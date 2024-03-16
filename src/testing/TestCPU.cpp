#include <iostream>
#include "TestCPU.h"
#include "../opcodes/opcodes.h"
#include "../cycles/cycles.h"
#include "../data_types/data_types.h"

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
