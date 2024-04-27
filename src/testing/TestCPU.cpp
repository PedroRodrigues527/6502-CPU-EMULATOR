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
    memory[0x0200] = opcodes::LDA;
    memory[0x0201] = 0x42;

    cpu.exec(cycles::LOAD_CYCLES, memory);

    if (cpu.getAcc() != 0x42)
    {
        std::cout << "LDA ERROR";
        return 1;
    }

    // TODO: test processor flags

    cpu.reset(memory);

    // LDA ZERO PAGE
    memory[0xff] = 0x42;
    memory[0x0200] = opcodes::LDA_ZERO_PAGE;
    memory[0x0201] = 0xff;

    cpu.exec(cycles::LOAD_ZERO_CYCLES, memory);

    if (cpu.getAcc() != 0x42)
    {
        std::cout << "LDA ZERO PAGE ERROR";
        return 1;
    }

    // TODO: test processor flags

    cpu.reset(memory);

    // LDA ZERO PAGE X
    cpu.setRX(0x11);
    memory[0x0200] = opcodes::LDA_ZERO_PAGE_X;
    memory[0x0201] = 0x20;

    memory[0x31] = 0x22;

    cpu.exec(cycles::LOAD_ZERO_X_CYCLES, memory);

    if (cpu.getAcc() != 0x22)
    {
        std::cout << "\nLDA ZERO PAGE X ERROR\n";
        return 1;
    }

    // TODO: test processor flags

    cpu.reset(memory);

    // LDX
    memory[0x0200] = opcodes::LDX;
    memory[0x0201] = 0x42;

    cpu.exec(cycles::LOAD_CYCLES, memory);

    if (cpu.getRX() != 0x42)
    {
        std::cout << "LDX ERROR";
        return 1;
    }

    // TODO: test processor flags

    cpu.reset(memory);

    // LDX ZERO PAGE
    memory[0x0200] = opcodes::LDX_ZERO_PAGE;
    memory[0x0201] = 0xff;
    memory[0xff] = 0x42;

    cpu.exec(cycles::LOAD_ZERO_CYCLES, memory);

    if (cpu.getRX() != 0x42)
    {
        std::cout << "LDX ZERO PAGE ERROR";
        return 1;
    }

    // TODO: test processor flags

    cpu.reset(memory);

    // LDY
    memory[0x0200] = opcodes::LDY;
    memory[0x0201] = 0x42;

    cpu.exec(cycles::LOAD_CYCLES, memory);

    if (cpu.getRY() != 0x42)
    {
        std::cout << "LDY ERROR";
        return 1;
    }

    // TODO: test processor flags

    cpu.reset(memory);

    // LDY ZERO PAGE

    memory[0x0200] = opcodes::LDY_ZERO_PAGE;
    memory[0x0201] = 0xff;
    memory[0xff] = 0x42;

    cpu.exec(cycles::LOAD_CYCLES, memory);

    if (cpu.getRY() != 0x42)
    {
        std::cout << "LDY ZERO PAGE ERROR";
        return 1;
    }

    // TODO: test processor flags

    cpu.reset(memory);

    // JSR
    memory[0x0200] = opcodes::JSR;
    memory[0x0201] = 0x41;
    memory[0x0202] = 0x42;
    memory[0x4241] = opcodes::LDA;
    memory[0x4242] = 0x30;

    cpu.exec(cycles::JSR_CYCLES, memory);

    if (cpu.getAcc() != 0x30)
    {
        std::cout << "JSR ERROR";
        return 1;
    }
 
    cpu.reset(memory);

    // STA ZERO PAGE
    cpu.setAcc(0x23);
    memory[0x0200] = opcodes::STA_ZERO_PAGE;
    memory[0x0201] = 0xfb;

    cpu.exec(cycles::STA_ZERO_PAGE, memory);

    if (memory[0xfb] != cpu.getAcc())
    {
        std::cout << "STA ERROR";
        return 1;
    }

    cpu.reset(memory);

    // STA ZERO PAGE X
    cpu.setAcc(0x31);
    cpu.setRX(0x8a);
    memory[0x0200] = opcodes::STA_ZERO_PAGE_X;
    memory[0x0201] =  0xfa;

    Word newAddr = cpu.getRX() + 0xfa;
  
    cpu.exec(cycles::STA_ZERO_PAGE_X, memory);

    if (memory[newAddr] != cpu.getAcc())
    {
        std::cout << (int)memory[newAddr] << std::endl;
        std::cout << "STA ZPX ERROR\n";
        return 1;
    }

    cpu.reset(memory);

    // STA ZERO PAGE ABSOLUTE
    cpu.setAcc(0x33);
    memory[0x0200] = opcodes::STA_ZERO_PAGE_ABSOLUTE;
    memory[0x0201] = 0xff;
    memory[0x0202] = 0xfA;
  
    cpu.exec(cycles::STA_ZERO_PAGE_ABSOLUTE, memory);

    if (memory[0xfaff] != cpu.getAcc())
    {
        std::cout << "STA ABS ERROR\n";
        return 1;
    }

    cpu.reset(memory);
    newAddr = 0;

    // STA ZERO PAGE ABSOLUTE X
    cpu.setAcc(0x33);
    cpu.setRX(0x8a);
    memory[0x0200] = opcodes::STA_ZERO_PAGE_ABSOLUTE_X;
    memory[0x0201] = 0xff;
    memory[0x0202] = 0xfA;

    newAddr = cpu.getRX() + 0xfaff;
  
    cpu.exec(cycles::STA_ZERO_PAGE_ABSOLUTE_X, memory);

    if (memory[newAddr] != cpu.getAcc())
    {
        std::cout << "STA ABS X ERROR\n";
        return 1;
    }

    cpu.reset(memory);
    newAddr = 0;

    // STA ZERO PAGE ABSOLUTE X
    cpu.setAcc(0x33);
    cpu.setRY(0x8a);
    memory[0x0200] = opcodes::STA_ZERO_PAGE_ABSOLUTE_Y;
    memory[0x0201] = 0xff;
    memory[0x0202] = 0xfA;

    newAddr = cpu.getRY() + 0xfaff;
  
    cpu.exec(cycles::STA_ZERO_PAGE_ABSOLUTE_Y, memory);

    if (memory[newAddr] != cpu.getAcc())
    {
        std::cout << "STA ABS Y ERROR\n";
        return 1;
    }

    cpu.reset(memory);
    newAddr = 0;

    // STX ZERO PAGE
    cpu.setRX(0x23);
    memory[0x0200] = opcodes::STX_ZERO_PAGE;
    memory[0x0201] = 0xfb;

    cpu.exec(cycles::STX_ZERO_PAGE, memory);

    if (memory[0xfb] != cpu.getRX())
    {
        std::cout << "STX ERROR";
        return 1;
    }

    cpu.reset(memory);

    // STY ZERO PAGE
    cpu.setRY(0x23);
    memory[0x0200] = opcodes::STY_ZERO_PAGE;
    memory[0x0201] = 0xfb;

    cpu.exec(cycles::STY_ZERO_PAGE, memory);

    if (memory[0xfb] != cpu.getRY())
    {
        std::cout << "STY ERROR";
        return 1;
    }
    
    std::cout << "\nALL TESTS PASSED!\n";
    return 0;
}
