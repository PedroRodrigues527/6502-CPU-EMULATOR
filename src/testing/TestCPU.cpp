#include <iostream>
#include <unistd.h>
#include "TestCPU.h"
#include "../opcodes/opcodes.h"
#include "../cycles/cycles.h"
#include "../data_types/data_types.h"
#include "../assembler/Assembler.h"

/**
 * Run's tests for every operation avaliable in this emulator. Verifies CPU operations
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool isCPUWithErrors(Memory &memory, CPU &cpu)
{
    std::cout << "TESTING CPU...\n";

    if (
        LDA_test(memory, cpu)                       ||
        LDA_ZERO_PAGE_test(memory, cpu)             ||
        LDA_ZERO_PAGE_X_test(memory, cpu)           ||
        LDX_test(memory, cpu)                       ||
        LDX_ZERO_PAGE_test(memory, cpu)             ||
        LDY_test(memory, cpu)                       ||
        LDY_ZERO_PAGE_test(memory, cpu)             ||
        JSR_test(memory, cpu)                       ||
        STA_ZERO_PAGE_test(memory, cpu)             ||
        STA_ZERO_PAGE_X_test(memory, cpu)           ||
        STA_ZERO_PAGE_ABSOLUTE_test(memory, cpu)    ||
        STA_ZERO_PAGE_ABSOLUTE_X_test(memory, cpu)  ||
        STA_ZERO_PAGE_ABSOLUTE_Y_test(memory, cpu)  ||
        STX_ZERO_PAGE_test(memory, cpu)             ||
        assembler_test(memory, cpu)                 ||
        STY_ZERO_PAGE_test(memory, cpu))            
    {
        return 1;
    }
    std::cout << "\nALL TESTS PASSED!" << std::endl;
    return 0;
}

/**
 * Run's tests for LDA operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDA_test(Memory &memory, CPU &cpu)
{
    std::cout << "LDA TEST ";
    memory[0x0200] = opcodes::LDA;
    memory[0x0201] = 0x42;

    cpu.exec(memory);

    if (cpu.getAcc() != 0x42)
    {
        std::cout << RED << "ERROR" << RESET << std::endl;
        return 1;
    }

    std::cout << GREEN << "SUCCESS" << RESET << std::endl;

    // TODO: test processor flags
    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for LDA ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDA_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    std::cout << "LDA ZERO PAGE TEST ";
    memory[0xff] = 0x42;
    memory[0x0200] = opcodes::LDA_ZERO_PAGE;
    memory[0x0201] = 0xff;

    cpu.exec(memory);

    if (cpu.getAcc() != 0x42)
    {
        std::cout << RED << "ERROR" << RESET << std::endl;
        return 1;
    }

    std::cout << GREEN << "SUCCESS" << RESET << std::endl;

    // TODO: test processor flags
    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for LDA ZERO PAGE X operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDA_ZERO_PAGE_X_test(Memory &memory, CPU &cpu)
{
    std::cout << "LDA ZERO PAGE X TEST ";
    cpu.setRX(0x11);
    memory[0x0200] = opcodes::LDA_ZERO_PAGE_X;
    memory[0x0201] = 0x20;

    memory[0x31] = 0x22;

    cpu.exec(memory);

    if (cpu.getAcc() != 0x22)
    {
        std::cout << RED << "ERROR" << RESET << std::endl;

        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    // TODO: test processor flags
    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for LDX operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDX_test(Memory &memory, CPU &cpu)
{
    std::cout << "LDX TEST ";
    memory[0x0200] = opcodes::LDX;
    memory[0x0201] = 0x42;

    cpu.exec(memory);

    if (cpu.getRX() != 0x42)
    {
        std::cout << RED << "ERROR" << RESET << std::endl;

        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    // TODO: test processor flags
    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for LDX ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDX_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    std::cout << "LDX ZERO PAGE TEST ";
    memory[0x0200] = opcodes::LDX_ZERO_PAGE;
    memory[0x0201] = 0xff;
    memory[0xff] = 0x42;

    cpu.exec(memory);

    if (cpu.getRX() != 0x42)
    {
        std::cout << RED << "ERROR" << RESET << std::endl;

        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    // TODO: test processor flags
    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for LDY operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDY_test(Memory &memory, CPU &cpu)
{
    std::cout << "LDY TEST ";
    memory[0x0200] = opcodes::LDY;
    memory[0x0201] = 0x42;

    cpu.exec(memory);

    if (cpu.getRY() != 0x42)
    {
        std::cout << RED << "ERROR" << RESET << std::endl;

        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    // TODO: test processor flags
    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for LDY ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool LDY_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    std::cout << "LDY ZERO PAGE TEST ";
    memory[0x0200] = opcodes::LDY_ZERO_PAGE;
    memory[0x0201] = 0xff;
    memory[0xff] = 0x42;

    cpu.exec(memory);

    if (cpu.getRY() != 0x42)
    {
        std::cout << RED << "ERROR" << RESET << std::endl;

        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    // TODO: test processor flags

    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for JSR operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool JSR_test(Memory &memory, CPU &cpu)
{
    std::cout << "JSR TEST ";
    memory[0x0200] = opcodes::JSR;
    memory[0x0201] = 0x41;
    memory[0x0202] = 0x42;
    memory[0x4241] = opcodes::LDA;
    memory[0x4242] = 0x30;

    cpu.exec(memory);

    if (cpu.getAcc() != 0x30)
    {
        std::cout << RED << "ERROR" << RESET << std::endl;
        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for STA ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STA_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    std::cout << "STA ZERO PAGE TEST ";
    cpu.setAcc(0x23);
    memory[0x0200] = opcodes::STA_ZERO_PAGE;
    memory[0x0201] = 0xfb;

    cpu.exec(memory);

    if (memory[0xfb] != cpu.getAcc())
    {
        std::cout << RED << "ERROR" << RESET << std::endl;
        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for STA ZERO PAGE X operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STA_ZERO_PAGE_X_test(Memory &memory, CPU &cpu)
{
    std::cout << "STA ZERO PAGE X TEST ";
    cpu.setAcc(0x31);
    cpu.setRX(0x8a);
    memory[0x0200] = opcodes::STA_ZERO_PAGE_X;
    memory[0x0201] = 0xfa;

    Word newAddr = cpu.getRX() + 0xfa;

    cpu.exec(memory);

    if (memory[newAddr] != cpu.getAcc())
    {
        std::cout << (int)memory[newAddr] << std::endl;
        std::cout << RED << "ERROR" << RESET << std::endl;

        return 1;
    }

    cpu.reset(memory);
    newAddr = 0;
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    return 0;
}

/**
 * Run's tests for STA ZERO PAGE ABSOLUTE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STA_ZERO_PAGE_ABSOLUTE_test(Memory &memory, CPU &cpu)
{
    std::cout << "STA ZERO PAGE ABSOLUTE TEST ";
    cpu.setAcc(0x33);
    memory[0x0200] = opcodes::STA_ZERO_PAGE_ABSOLUTE;
    memory[0x0201] = 0xff;
    memory[0x0202] = 0xfA;

    cpu.exec(memory);

    if (memory[0xfaff] != cpu.getAcc())
    {
        std::cout << RED << "ERROR" << RESET << std::endl;

        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for STA ZERO PAGE ABSOLUTE X operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STA_ZERO_PAGE_ABSOLUTE_X_test(Memory &memory, CPU &cpu)
{
    std::cout << "STA ZERO PAGE ABSOLUTE X TEST ";
    cpu.setAcc(0x33);
    cpu.setRX(0x8a);
    memory[0x0200] = opcodes::STA_ZERO_PAGE_ABSOLUTE_X;
    memory[0x0201] = 0xff;
    memory[0x0202] = 0xfA;

    Word newAddr = cpu.getRX() + 0xfaff;

    cpu.exec(memory);

    if (memory[newAddr] != cpu.getAcc())
    {
        std::cout << RED << "ERROR" << RESET << std::endl;

        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    cpu.reset(memory);
    newAddr = 0;
    return 0;
}

/**
 * Run's tests for STA ZERO PAGE ABSOLUTE Y operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STA_ZERO_PAGE_ABSOLUTE_Y_test(Memory &memory, CPU &cpu)
{
    std::cout << "STA ZERO PAGE ABSOLUTE Y TEST ";
    cpu.setAcc(0x33);
    cpu.setRY(0x8a);
    memory[0x0200] = opcodes::STA_ZERO_PAGE_ABSOLUTE_Y;
    memory[0x0201] = 0xff;
    memory[0x0202] = 0xfA;

    Word newAddr = cpu.getRY() + 0xfaff;

    cpu.exec(memory);

    if (memory[newAddr] != cpu.getAcc())
    {
        std::cout << RED << "ERROR" << RESET << std::endl;

        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    cpu.reset(memory);
    newAddr = 0;

    return 0;
}

/**
 * Run's tests for STX ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STX_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    std::cout << "STX ZERO PAGE TEST ";
    cpu.setRX(0x23);
    memory[0x0200] = opcodes::STX_ZERO_PAGE;
    memory[0x0201] = 0xfb;

    cpu.exec(memory);

    if (memory[0xfb] != cpu.getRX())
    {
        std::cout << RED << "ERROR" << RESET << std::endl;
        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for STY ZERO PAGE operation
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool STY_ZERO_PAGE_test(Memory &memory, CPU &cpu)
{
    std::cout << "STY ZERO PAGE TEST ";
    cpu.setRY(0x23);
    memory[0x0200] = opcodes::STY_ZERO_PAGE;
    memory[0x0201] = 0xfb;

    cpu.exec(memory);

    if (memory[0xfb] != cpu.getRY())
    {
        std::cout << RED << "ERROR" << RESET << std::endl;
        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    cpu.reset(memory);
    return 0;
}

/**
 * Run's tests for the assembler
 * @param Memory &memory
 * @param CPU &cpu
 * @returns bool true if CPU has problems
 */
bool assembler_test(Memory &memory, CPU &cpu)
{
    std::cout << "ASSEMBLER TEST ";
    compileAssemblyProgram(cpu, memory, "assembly_code_unit_test.txt");

    cpu.exec(memory);

    if (cpu.getAcc() != 0x32 || cpu.getRX() != 0x0A)
    {
        std::cout << RED << "ERROR" << RESET << std::endl;
        return 1;
    }
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
    cpu.reset(memory);
    return 0;
}

