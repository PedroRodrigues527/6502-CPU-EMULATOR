#include "Assembler.h"
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "../opcodes/opcodes.h"
#include "../cycles/cycles.h"
#include "../data_types/data_types.h"

Byte getOpcode(std::string instruction, bool isAddress)
{
    if (instruction == "LDA")
        return isAddress ? opcodes::LDA_ZERO_PAGE : opcodes::LDA;
    if (instruction == "LDX")
        return isAddress ? opcodes::LDX_ZERO_PAGE : opcodes::LDX;
    if (instruction == "LDY")
        return isAddress ? opcodes::LDY_ZERO_PAGE : opcodes::LDY;
    if (instruction == "STA")
        return isAddress ? opcodes::STA_ZERO_PAGE_ABSOLUTE : opcodes::STA_ZERO_PAGE;
    if (instruction == "JSR")
        return opcodes::JSR;
    // todo: more
    return 0x00; // NOP or unknown instruction
}

void compileAssemblyProgram(CPU &cpu, Memory &memory, std::string assembly_code_file) // assuming nothing is allocated from $0200 to $ffff
{
    int allocating_position = 0x0200;
    int cycles = 0;
    Byte opcode;

    std::fstream assembly_file;
    assembly_file.open(assembly_code_file, std::ios::in);

    if (assembly_file.is_open())
    {
        std::string line;
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

            // TODO: verify when to increment the allocating_position eg, in JSR
        }

        assembly_file.close();
    }
}