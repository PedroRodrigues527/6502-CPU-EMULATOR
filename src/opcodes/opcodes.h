#ifndef OPCODES_H
#define OPCODES_H

#include <unordered_map>

namespace opcodes
{
    using Byte = unsigned char; // 1 byte (8 bits)
    using Word = unsigned short; // 2 bytes (16 bits)

    // Load to accumulator
    constexpr Byte LDA = 0xA9; // immediate mode
    constexpr Byte LDA_ZERO_PAGE = 0xA5;
    constexpr Byte LDA_ZERO_PAGE_X = 0xB5;

    // Load to Register X
    constexpr Byte LDX = 0xA2;
    constexpr Byte LDX_ZERO_PAGE = 0xA6;

    // Load to Register Y
    constexpr Byte LDY = 0xA0;
    constexpr Byte LDY_ZERO_PAGE = 0xA4;

    constexpr Byte JSR = 0x20; // Jump to Subroutine

    constexpr Word STA_ZERO_PAGE = 0x85;
    constexpr Byte STA_ZERO_PAGE_X = 0x95;
    constexpr Word STA_ZERO_PAGE_ABSOLUTE = 0x8D;
    constexpr Byte STA_ZERO_PAGE_ABSOLUTE_X = 0x9D;
    constexpr Byte STA_ZERO_PAGE_ABSOLUTE_Y = 0x99;
    constexpr Byte STA_ZERO_PAGE_INDIRECT_X = 0x81;
    constexpr Byte STA_ZERO_PAGE_INDIRECT_Y = 0x91;

    constexpr Byte STX_ZERO_PAGE = 0x86;
    constexpr Byte STX_ZERO_PAGE_Y = 0x96;
    constexpr Byte STX_ABSOLUTE = 0x8E;

    constexpr Byte STY_ZERO_PAGE = 0x84;
    constexpr Byte STY_ZERO_PAGE_X = 0x94;
    constexpr Byte STY_ABSOLUTE = 0x8C;

    const std::unordered_map<Byte, int> opcode_map = {
        {opcodes::LDA, 2},
        {opcodes::LDA_ZERO_PAGE, 3},
        {opcodes::LDA_ZERO_PAGE_X, 4},
        {opcodes::LDX, 2},
        {opcodes::LDX_ZERO_PAGE, 3},
        {opcodes::LDY, 2},
        {opcodes::LDY_ZERO_PAGE, 3},
        {opcodes::JSR, 6},
        {opcodes::STA_ZERO_PAGE, 3},
        {opcodes::STA_ZERO_PAGE_X, 4},
        {opcodes::STA_ZERO_PAGE_ABSOLUTE, 4},
        {opcodes::STA_ZERO_PAGE_ABSOLUTE_X, 5},
        {opcodes::STA_ZERO_PAGE_ABSOLUTE_Y, 5},
        {opcodes::STA_ZERO_PAGE_INDIRECT_X, 6},
        {opcodes::STA_ZERO_PAGE_INDIRECT_Y, 6},
        {opcodes::STX_ZERO_PAGE, 3},
        {opcodes::STX_ZERO_PAGE_Y, 4},
        {opcodes::STX_ABSOLUTE, 4},
        {opcodes::STY_ZERO_PAGE, 3},
        {opcodes::STY_ZERO_PAGE_X, 4},
        {opcodes::STY_ABSOLUTE, 4}
    };

}

#endif