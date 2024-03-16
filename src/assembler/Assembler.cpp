#include "Assembler.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>
#include <fstream>
#include <string>
#include "../opcodes/opcodes.h"
#include "../cycles/cycles.h"
#include "../data_types/data_types.h"

Byte getOpcode(std::string instruction, bool isAddress)
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
}

int getCycles(std::string instruction, bool isAddress)
{
    if (instruction == "LDA" || instruction == "LDX" )
    {
        if (isAddress)
            return cycles::LOAD_ZERO_CYCLES;
        return cycles::LOAD_CYCLES;
    }

    return 0;
}

void compileAssemblyProgram(CPU &cpu, Memory &memory, std::string line) // assuming nothing is allocated from $0200 to $ffff
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
}