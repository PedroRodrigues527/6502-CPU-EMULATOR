#include <iostream>
#include <unistd.h>
#include "TestCPU.h"
#include "../opcodes/opcodes.h"
#include "../cycles/cycles.h"
#include "../data_types/data_types.h"

/**
 * Run's tests for every operation avaliable in this emulator. Verifies CPU operations
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
*/
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

    cpu.exec(cycles::LOAD_CYCLES, memory);

    if (cpu.RY != 0x42)
    {
        std::cout << "LDY ZERO PAGE ERROR";
        return 1;
    }

    cpu.reset(memory);

    // JSR
    memory[0xfffC] = opcodes::JSR;
    memory[0xfffD] = 0x41;
    memory[0xfffe] = 0x42;
    memory[0x4241] = opcodes::LDA;
    memory[0x4242] = 0x30;

    cpu.exec(cycles::JSR_CYCLES, memory);

    if (cpu.Acc != 0x30)
    {
        std::cout << "JSR ERROR";
        return 1;
    }
 
    cpu.reset(memory);

    // STA ZERO PAGE
    cpu.Acc = 0x23;
    memory[0xfffC] = opcodes::STA_ZERO_PAGE;
    memory[0xfffD] = 0xfb;

    cpu.exec(cycles::STA_ZERO_PAGE, memory);

    if (memory[0xfb] != cpu.Acc)
    {
        std::cout << "STA ERROR";
        return 1;
    }

    cpu.reset(memory);

    // STA ZERO PAGE X
    cpu.Acc = 0x31;
    cpu.RX = 0x8a;
    memory[0xfffC] = opcodes::STA_ZERO_PAGE_X;
    memory[0xfffD] =  0xfa;

    Word newAddr = (Word)0x8a + (Word)0xfa;
  
    cpu.exec(cycles::STA_ZERO_PAGE_X, memory);

    if ((Word)memory[newAddr] != (Word)cpu.Acc)
    {
        std::cout << (int)memory[newAddr] << std::endl;
        std::cout << "STA ZPX ERROR\n";
        return 1;
    }

    cpu.reset(memory);

    // STA ZERO PAGE ABSOLUTE
    cpu.Acc = 0x33;
    memory[0xfffC] = opcodes::STA_ZERO_PAGE_ABSOLUTE;
    memory[0xfffD] = 0xff;
    memory[0xfffE] = 0xfA;
  
    cpu.exec(cycles::STA_ZERO_PAGE_ABSOLUTE, memory);

    if (memory[0xfaff] != cpu.Acc)
    {
        std::cout << "STA ZPA ERROR\n";
        return 1;
    }

    cpu.reset(memory);
    std::cout << "\nALL TESTS PASSED!\n";
    return 0;
}
