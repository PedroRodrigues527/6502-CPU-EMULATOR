#ifndef OPCODES_H
#define OPCODES_H

namespace opcodes
{
    using Byte = unsigned char;  // 1 byte (8 bits)
    
    constexpr Byte LDA = 0xA9; // immediate mode
    constexpr Byte LDA_ZERO_PAGE = 0xA5; // immediate mode
    constexpr Byte LDA_ZERO_PAGE_X = 0xB5; // immediate mode

    constexpr Byte LDX = 0xA2;
    constexpr Byte LDY = 0xA0;
}
#endif