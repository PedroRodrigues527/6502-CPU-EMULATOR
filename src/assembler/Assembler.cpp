#include "Assembler.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

#include "../opcodes/opcodes.h"
#include "../cycles/cycles.h"
#include "../data_types/data_types.h"


/**
 * Remove spaces from each line
 */
static inline std::string trim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

/**
 * Converts assembly 'values' into bytes. Example: "$20" to (byte)20
 */
Byte parseValue(const std::string &valStr)
{
    std::string v = valStr;

    if (!v.empty() && v[0] == '$')
        v = v.substr(1);

    if (!v.empty() && v[0] == '#')
        v = v.substr(1);

    return static_cast<Byte>(std::stoi(v, nullptr, 10));
}

/**
 * Get opcode from given instruction.
 */
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

    std::cerr << "[Assembler] Unknown instruction: " << instruction << "\n";
    return 0x00; // NOP
}

void compileAssemblyProgram(CPU &cpu, Memory &memory, std::string assembly_code_file)
{
    int allocating_position = 0x0200;

    std::ifstream assembly_file(assembly_code_file);

    if (!assembly_file.is_open())
    {
        std::cerr << "[Assembler] ERROR: Cannot open file " << assembly_code_file << "\n";
        return;
    }

    std::string line;
    while (getline(assembly_file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == ';')
            continue;

        std::stringstream ss(line);
        std::string mnemonic, operand;

        ss >> mnemonic;   // ex: LDA
        ss >> operand;    // ex: $20

        if (mnemonic.empty())
            continue;

        bool hasOperand = !operand.empty();
        bool isAddress = false;

        if (hasOperand && operand[0] == '$')
            isAddress = true;

        Byte opcode = getOpcode(mnemonic, isAddress);
        memory[allocating_position++] = opcode;

        if (!hasOperand)
            continue;

        // 1 Byte instruction
        if (mnemonic != "JSR")
        {
            Byte value = parseValue(operand);
            memory[allocating_position++] = value;
        }
        else
        {
            // ===========================
            //  JSR → 2 bytes (little endian)
            // ===========================
            Word address = parseValue(operand);
            Byte lo = address & 0xFF;
            Byte hi = (address >> 8) & 0xFF;

            memory[allocating_position++] = lo;
            memory[allocating_position++] = hi;
        }
    }

    assembly_file.close();
}
