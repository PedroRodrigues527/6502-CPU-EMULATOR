#ifndef OPCODES_H
#define OPCODES_H

namespace opcodes
{
    using Byte = unsigned char; // 1 byte (8 bits)

    // Load to accumulator (Acc)
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
}
#endif