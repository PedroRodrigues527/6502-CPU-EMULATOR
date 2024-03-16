#include "opcodes/opcodes.h"
#include "data_types/data_types.h"
#include "components/Memory.h"
#include "components/Cpu.h"
#include "testing/TestCPU.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>
#include <fstream>
#include <string>

/* void loadTestProgram(Memory &memory)
{
    memory[0xfffC] = opcodes::JSR;
    memory[0xfffD] = 0x42;
    memory[0xfffe] = 0x42;
    memory[0x4242] = opcodes::LDA;
    memory[0x4243] = 0x84;
} */

/* Byte getOpcode(std::string instruction, bool isAddress)
{
    if (instruction == "LDA")
    {
        if (isAddress)
            return opcodes::LDA_ZERO_PAGE;
        return opcodes::LDA;
    }
    if (instruction == "LDX")
    {
        if (isAddress)
            return opcodes::LDX_ZERO_PAGE;
        return opcodes::LDX;
    }
    return 0x00;
} */

/* int getCycles(std::string instruction, bool isAddress)
{
    if (instruction == "LDA" || instruction == "LDX" )
    {
        if (isAddress)
            return cycles::LOAD_ZERO_CYCLES;
        return cycles::LOAD_CYCLES;
    }

    return 0;
} */

/* void compileAssemblyProgram(CPU &cpu, Memory &memory, std::string line) // assuming nothing is allocated from $0200 to $ffff
{
    int allocating_position = 0xfffC;

    int cycles = 0;

    Byte opcode;

    std::fstream assembly_file;

    assembly_file.open("assembly_code.txt", std::ios::in);

    if (assembly_file.is_open())
    {
        while (getline(assembly_file, line))
        {
            std::string asm_instruction = line.substr(0, line.find(' '));

            std::string asm_value = line.substr(line.find(' ') + 1, line.length());

            std::string value = asm_value.substr(1, asm_value.length());

            bool isAddress = (char)asm_value[0] == (char)'$';

            memory[allocating_position] = getOpcode(asm_instruction, isAddress);
            allocating_position++;
            memory[allocating_position] = std::stoi(value);
            allocating_position++;

            cpu.exec(getCycles(asm_instruction, isAddress), memory);
            // TODO: verify when to increment the allocating_position eg, in JSR
        }

        assembly_file.close();
    }
} */

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
        std::string line_code;
        /* loadTestProgram(memory); */
        /* compileAssemblyProgram(cpu, memory, line_code); */
        /* cpu.exec(cycles::JSR_CYCLES, memory); // verify number of cycles */
        return 0;
    }
    return 0;
};