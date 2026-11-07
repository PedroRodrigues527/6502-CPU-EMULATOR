#ifndef OPCODES_H
#define OPCODES_H

#include <unordered_map>

/**
 * All 151 official 6502 opcodes.
 *
 * Naming convention (existing names were kept so the assembler keeps working):
 *   <MNEMONIC>                 immediate mode (or implied, for instructions without an operand)
 *   <MNEMONIC>_ACCUMULATOR     accumulator mode
 *   <MNEMONIC>_ZERO_PAGE       zero page            $nn
 *   <MNEMONIC>_ZERO_PAGE_X     zero page,X          $nn,X
 *   <MNEMONIC>_ZERO_PAGE_Y     zero page,Y          $nn,Y
 *   <MNEMONIC>_ABSOLUTE        absolute             $nnnn
 *   <MNEMONIC>_ABSOLUTE_X      absolute,X           $nnnn,X
 *   <MNEMONIC>_ABSOLUTE_Y      absolute,Y           $nnnn,Y
 *   <MNEMONIC>_INDIRECT_X      (indirect,X)         ($nn,X)
 *   <MNEMONIC>_INDIRECT_Y      (indirect),Y         ($nn),Y
 *   JMP_INDIRECT               indirect             ($nnnn)
 *   B<cc>                      relative branches
 */
namespace opcodes
{
    using Byte = unsigned char; // 1 byte (8 bits)
    using Word = unsigned short; // 2 bytes (16 bits)

    // ---------------------------------------------------------------- Loads
    // Load to accumulator
    constexpr Byte LDA = 0xA9; // immediate mode
    constexpr Byte LDA_ZERO_PAGE = 0xA5;
    constexpr Byte LDA_ZERO_PAGE_X = 0xB5;
    constexpr Byte LDA_ABSOLUTE = 0xAD;
    constexpr Byte LDA_ABSOLUTE_X = 0xBD;
    constexpr Byte LDA_ABSOLUTE_Y = 0xB9;
    constexpr Byte LDA_INDIRECT_X = 0xA1;
    constexpr Byte LDA_INDIRECT_Y = 0xB1;

    // Load to Register X
    constexpr Byte LDX = 0xA2;
    constexpr Byte LDX_ZERO_PAGE = 0xA6;
    constexpr Byte LDX_ZERO_PAGE_Y = 0xB6;
    constexpr Byte LDX_ABSOLUTE = 0xAE;
    constexpr Byte LDX_ABSOLUTE_Y = 0xBE;

    // Load to Register Y
    constexpr Byte LDY = 0xA0;
    constexpr Byte LDY_ZERO_PAGE = 0xA4;
    constexpr Byte LDY_ZERO_PAGE_X = 0xB4;
    constexpr Byte LDY_ABSOLUTE = 0xAC;
    constexpr Byte LDY_ABSOLUTE_X = 0xBC;

    // --------------------------------------------------------------- Stores
    constexpr Byte STA_ZERO_PAGE = 0x85;
    constexpr Byte STA_ZERO_PAGE_X = 0x95;
    constexpr Byte STA_ZERO_PAGE_ABSOLUTE = 0x8D;
    constexpr Byte STA_ZERO_PAGE_ABSOLUTE_X = 0x9D;
    constexpr Byte STA_ZERO_PAGE_ABSOLUTE_Y = 0x99;
    constexpr Byte STA_ZERO_PAGE_INDIRECT_X = 0x81;
    constexpr Byte STA_ZERO_PAGE_INDIRECT_Y = 0x91;
    // Aliases with the standard naming (same opcodes as the ones above)
    constexpr Byte STA_ABSOLUTE = STA_ZERO_PAGE_ABSOLUTE;
    constexpr Byte STA_ABSOLUTE_X = STA_ZERO_PAGE_ABSOLUTE_X;
    constexpr Byte STA_ABSOLUTE_Y = STA_ZERO_PAGE_ABSOLUTE_Y;
    constexpr Byte STA_INDIRECT_X = STA_ZERO_PAGE_INDIRECT_X;
    constexpr Byte STA_INDIRECT_Y = STA_ZERO_PAGE_INDIRECT_Y;

    constexpr Byte STX_ZERO_PAGE = 0x86;
    constexpr Byte STX_ZERO_PAGE_Y = 0x96;
    constexpr Byte STX_ABSOLUTE = 0x8E;

    constexpr Byte STY_ZERO_PAGE = 0x84;
    constexpr Byte STY_ZERO_PAGE_X = 0x94;
    constexpr Byte STY_ABSOLUTE = 0x8C;

    // ------------------------------------------------------ Register transfers
    constexpr Byte TAX = 0xAA;
    constexpr Byte TAY = 0xA8;
    constexpr Byte TXA = 0x8A;
    constexpr Byte TYA = 0x98;
    constexpr Byte TSX = 0xBA;
    constexpr Byte TXS = 0x9A;

    // ---------------------------------------------------------------- Stack
    constexpr Byte PHA = 0x48;
    constexpr Byte PHP = 0x08;
    constexpr Byte PLA = 0x68;
    constexpr Byte PLP = 0x28;

    // -------------------------------------------------------------- Logical
    constexpr Byte AND = 0x29;
    constexpr Byte AND_ZERO_PAGE = 0x25;
    constexpr Byte AND_ZERO_PAGE_X = 0x35;
    constexpr Byte AND_ABSOLUTE = 0x2D;
    constexpr Byte AND_ABSOLUTE_X = 0x3D;
    constexpr Byte AND_ABSOLUTE_Y = 0x39;
    constexpr Byte AND_INDIRECT_X = 0x21;
    constexpr Byte AND_INDIRECT_Y = 0x31;

    constexpr Byte EOR = 0x49;
    constexpr Byte EOR_ZERO_PAGE = 0x45;
    constexpr Byte EOR_ZERO_PAGE_X = 0x55;
    constexpr Byte EOR_ABSOLUTE = 0x4D;
    constexpr Byte EOR_ABSOLUTE_X = 0x5D;
    constexpr Byte EOR_ABSOLUTE_Y = 0x59;
    constexpr Byte EOR_INDIRECT_X = 0x41;
    constexpr Byte EOR_INDIRECT_Y = 0x51;

    constexpr Byte ORA = 0x09;
    constexpr Byte ORA_ZERO_PAGE = 0x05;
    constexpr Byte ORA_ZERO_PAGE_X = 0x15;
    constexpr Byte ORA_ABSOLUTE = 0x0D;
    constexpr Byte ORA_ABSOLUTE_X = 0x1D;
    constexpr Byte ORA_ABSOLUTE_Y = 0x19;
    constexpr Byte ORA_INDIRECT_X = 0x01;
    constexpr Byte ORA_INDIRECT_Y = 0x11;

    constexpr Byte BIT_ZERO_PAGE = 0x24;
    constexpr Byte BIT_ABSOLUTE = 0x2C;

    // ----------------------------------------------------------- Arithmetic
    constexpr Byte ADC = 0x69;
    constexpr Byte ADC_ZERO_PAGE = 0x65;
    constexpr Byte ADC_ZERO_PAGE_X = 0x75;
    constexpr Byte ADC_ABSOLUTE = 0x6D;
    constexpr Byte ADC_ABSOLUTE_X = 0x7D;
    constexpr Byte ADC_ABSOLUTE_Y = 0x79;
    constexpr Byte ADC_INDIRECT_X = 0x61;
    constexpr Byte ADC_INDIRECT_Y = 0x71;

    constexpr Byte SBC = 0xE9;
    constexpr Byte SBC_ZERO_PAGE = 0xE5;
    constexpr Byte SBC_ZERO_PAGE_X = 0xF5;
    constexpr Byte SBC_ABSOLUTE = 0xED;
    constexpr Byte SBC_ABSOLUTE_X = 0xFD;
    constexpr Byte SBC_ABSOLUTE_Y = 0xF9;
    constexpr Byte SBC_INDIRECT_X = 0xE1;
    constexpr Byte SBC_INDIRECT_Y = 0xF1;

    constexpr Byte CMP = 0xC9;
    constexpr Byte CMP_ZERO_PAGE = 0xC5;
    constexpr Byte CMP_ZERO_PAGE_X = 0xD5;
    constexpr Byte CMP_ABSOLUTE = 0xCD;
    constexpr Byte CMP_ABSOLUTE_X = 0xDD;
    constexpr Byte CMP_ABSOLUTE_Y = 0xD9;
    constexpr Byte CMP_INDIRECT_X = 0xC1;
    constexpr Byte CMP_INDIRECT_Y = 0xD1;

    constexpr Byte CPX = 0xE0;
    constexpr Byte CPX_ZERO_PAGE = 0xE4;
    constexpr Byte CPX_ABSOLUTE = 0xEC;

    constexpr Byte CPY = 0xC0;
    constexpr Byte CPY_ZERO_PAGE = 0xC4;
    constexpr Byte CPY_ABSOLUTE = 0xCC;

    // --------------------------------------------- Increments & decrements
    constexpr Byte INC_ZERO_PAGE = 0xE6;
    constexpr Byte INC_ZERO_PAGE_X = 0xF6;
    constexpr Byte INC_ABSOLUTE = 0xEE;
    constexpr Byte INC_ABSOLUTE_X = 0xFE;

    constexpr Byte DEC_ZERO_PAGE = 0xC6;
    constexpr Byte DEC_ZERO_PAGE_X = 0xD6;
    constexpr Byte DEC_ABSOLUTE = 0xCE;
    constexpr Byte DEC_ABSOLUTE_X = 0xDE;

    constexpr Byte INX = 0xE8;
    constexpr Byte INY = 0xC8;
    constexpr Byte DEX = 0xCA;
    constexpr Byte DEY = 0x88;

    // --------------------------------------------------------------- Shifts
    constexpr Byte ASL_ACCUMULATOR = 0x0A;
    constexpr Byte ASL_ZERO_PAGE = 0x06;
    constexpr Byte ASL_ZERO_PAGE_X = 0x16;
    constexpr Byte ASL_ABSOLUTE = 0x0E;
    constexpr Byte ASL_ABSOLUTE_X = 0x1E;

    constexpr Byte LSR_ACCUMULATOR = 0x4A;
    constexpr Byte LSR_ZERO_PAGE = 0x46;
    constexpr Byte LSR_ZERO_PAGE_X = 0x56;
    constexpr Byte LSR_ABSOLUTE = 0x4E;
    constexpr Byte LSR_ABSOLUTE_X = 0x5E;

    constexpr Byte ROL_ACCUMULATOR = 0x2A;
    constexpr Byte ROL_ZERO_PAGE = 0x26;
    constexpr Byte ROL_ZERO_PAGE_X = 0x36;
    constexpr Byte ROL_ABSOLUTE = 0x2E;
    constexpr Byte ROL_ABSOLUTE_X = 0x3E;

    constexpr Byte ROR_ACCUMULATOR = 0x6A;
    constexpr Byte ROR_ZERO_PAGE = 0x66;
    constexpr Byte ROR_ZERO_PAGE_X = 0x76;
    constexpr Byte ROR_ABSOLUTE = 0x6E;
    constexpr Byte ROR_ABSOLUTE_X = 0x7E;

    // ---------------------------------------------------------------- Jumps
    constexpr Byte JMP_ABSOLUTE = 0x4C;
    constexpr Byte JMP_INDIRECT = 0x6C;
    constexpr Byte JSR = 0x20; // Jump to Subroutine
    constexpr Byte RTS = 0x60; // Return from Subroutine

    // ------------------------------------------------------------- Branches
    constexpr Byte BCC = 0x90;
    constexpr Byte BCS = 0xB0;
    constexpr Byte BEQ = 0xF0;
    constexpr Byte BMI = 0x30;
    constexpr Byte BNE = 0xD0;
    constexpr Byte BPL = 0x10;
    constexpr Byte BVC = 0x50;
    constexpr Byte BVS = 0x70;

    // ----------------------------------------------------- Status flag changes
    constexpr Byte CLC = 0x18;
    constexpr Byte CLD = 0xD8;
    constexpr Byte CLI = 0x58;
    constexpr Byte CLV = 0xB8;
    constexpr Byte SEC = 0x38;
    constexpr Byte SED = 0xF8;
    constexpr Byte SEI = 0x78;

    // --------------------------------------------------------------- System
    constexpr Byte BRK = 0x00;
    constexpr Byte NOP = 0xEA;
    constexpr Byte RTI = 0x40;

    /**
     * Base clock cycles of every opcode. Extra cycles (page crossing for indexed reads,
     * branches taken / crossing a page) are added while executing.
     */
    const std::unordered_map<Byte, int> opcode_map = {
        // Loads
        {opcodes::LDA, 2},
        {opcodes::LDA_ZERO_PAGE, 3},
        {opcodes::LDA_ZERO_PAGE_X, 4},
        {opcodes::LDA_ABSOLUTE, 4},
        {opcodes::LDA_ABSOLUTE_X, 4},
        {opcodes::LDA_ABSOLUTE_Y, 4},
        {opcodes::LDA_INDIRECT_X, 6},
        {opcodes::LDA_INDIRECT_Y, 5},
        {opcodes::LDX, 2},
        {opcodes::LDX_ZERO_PAGE, 3},
        {opcodes::LDX_ZERO_PAGE_Y, 4},
        {opcodes::LDX_ABSOLUTE, 4},
        {opcodes::LDX_ABSOLUTE_Y, 4},
        {opcodes::LDY, 2},
        {opcodes::LDY_ZERO_PAGE, 3},
        {opcodes::LDY_ZERO_PAGE_X, 4},
        {opcodes::LDY_ABSOLUTE, 4},
        {opcodes::LDY_ABSOLUTE_X, 4},

        // Stores
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
        {opcodes::STY_ABSOLUTE, 4},

        // Register transfers
        {opcodes::TAX, 2},
        {opcodes::TAY, 2},
        {opcodes::TXA, 2},
        {opcodes::TYA, 2},
        {opcodes::TSX, 2},
        {opcodes::TXS, 2},

        // Stack
        {opcodes::PHA, 3},
        {opcodes::PHP, 3},
        {opcodes::PLA, 4},
        {opcodes::PLP, 4},

        // Logical
        {opcodes::AND, 2},
        {opcodes::AND_ZERO_PAGE, 3},
        {opcodes::AND_ZERO_PAGE_X, 4},
        {opcodes::AND_ABSOLUTE, 4},
        {opcodes::AND_ABSOLUTE_X, 4},
        {opcodes::AND_ABSOLUTE_Y, 4},
        {opcodes::AND_INDIRECT_X, 6},
        {opcodes::AND_INDIRECT_Y, 5},
        {opcodes::EOR, 2},
        {opcodes::EOR_ZERO_PAGE, 3},
        {opcodes::EOR_ZERO_PAGE_X, 4},
        {opcodes::EOR_ABSOLUTE, 4},
        {opcodes::EOR_ABSOLUTE_X, 4},
        {opcodes::EOR_ABSOLUTE_Y, 4},
        {opcodes::EOR_INDIRECT_X, 6},
        {opcodes::EOR_INDIRECT_Y, 5},
        {opcodes::ORA, 2},
        {opcodes::ORA_ZERO_PAGE, 3},
        {opcodes::ORA_ZERO_PAGE_X, 4},
        {opcodes::ORA_ABSOLUTE, 4},
        {opcodes::ORA_ABSOLUTE_X, 4},
        {opcodes::ORA_ABSOLUTE_Y, 4},
        {opcodes::ORA_INDIRECT_X, 6},
        {opcodes::ORA_INDIRECT_Y, 5},
        {opcodes::BIT_ZERO_PAGE, 3},
        {opcodes::BIT_ABSOLUTE, 4},

        // Arithmetic
        {opcodes::ADC, 2},
        {opcodes::ADC_ZERO_PAGE, 3},
        {opcodes::ADC_ZERO_PAGE_X, 4},
        {opcodes::ADC_ABSOLUTE, 4},
        {opcodes::ADC_ABSOLUTE_X, 4},
        {opcodes::ADC_ABSOLUTE_Y, 4},
        {opcodes::ADC_INDIRECT_X, 6},
        {opcodes::ADC_INDIRECT_Y, 5},
        {opcodes::SBC, 2},
        {opcodes::SBC_ZERO_PAGE, 3},
        {opcodes::SBC_ZERO_PAGE_X, 4},
        {opcodes::SBC_ABSOLUTE, 4},
        {opcodes::SBC_ABSOLUTE_X, 4},
        {opcodes::SBC_ABSOLUTE_Y, 4},
        {opcodes::SBC_INDIRECT_X, 6},
        {opcodes::SBC_INDIRECT_Y, 5},
        {opcodes::CMP, 2},
        {opcodes::CMP_ZERO_PAGE, 3},
        {opcodes::CMP_ZERO_PAGE_X, 4},
        {opcodes::CMP_ABSOLUTE, 4},
        {opcodes::CMP_ABSOLUTE_X, 4},
        {opcodes::CMP_ABSOLUTE_Y, 4},
        {opcodes::CMP_INDIRECT_X, 6},
        {opcodes::CMP_INDIRECT_Y, 5},
        {opcodes::CPX, 2},
        {opcodes::CPX_ZERO_PAGE, 3},
        {opcodes::CPX_ABSOLUTE, 4},
        {opcodes::CPY, 2},
        {opcodes::CPY_ZERO_PAGE, 3},
        {opcodes::CPY_ABSOLUTE, 4},

        // Increments & decrements
        {opcodes::INC_ZERO_PAGE, 5},
        {opcodes::INC_ZERO_PAGE_X, 6},
        {opcodes::INC_ABSOLUTE, 6},
        {opcodes::INC_ABSOLUTE_X, 7},
        {opcodes::DEC_ZERO_PAGE, 5},
        {opcodes::DEC_ZERO_PAGE_X, 6},
        {opcodes::DEC_ABSOLUTE, 6},
        {opcodes::DEC_ABSOLUTE_X, 7},
        {opcodes::INX, 2},
        {opcodes::INY, 2},
        {opcodes::DEX, 2},
        {opcodes::DEY, 2},

        // Shifts
        {opcodes::ASL_ACCUMULATOR, 2},
        {opcodes::ASL_ZERO_PAGE, 5},
        {opcodes::ASL_ZERO_PAGE_X, 6},
        {opcodes::ASL_ABSOLUTE, 6},
        {opcodes::ASL_ABSOLUTE_X, 7},
        {opcodes::LSR_ACCUMULATOR, 2},
        {opcodes::LSR_ZERO_PAGE, 5},
        {opcodes::LSR_ZERO_PAGE_X, 6},
        {opcodes::LSR_ABSOLUTE, 6},
        {opcodes::LSR_ABSOLUTE_X, 7},
        {opcodes::ROL_ACCUMULATOR, 2},
        {opcodes::ROL_ZERO_PAGE, 5},
        {opcodes::ROL_ZERO_PAGE_X, 6},
        {opcodes::ROL_ABSOLUTE, 6},
        {opcodes::ROL_ABSOLUTE_X, 7},
        {opcodes::ROR_ACCUMULATOR, 2},
        {opcodes::ROR_ZERO_PAGE, 5},
        {opcodes::ROR_ZERO_PAGE_X, 6},
        {opcodes::ROR_ABSOLUTE, 6},
        {opcodes::ROR_ABSOLUTE_X, 7},

        // Jumps
        {opcodes::JMP_ABSOLUTE, 3},
        {opcodes::JMP_INDIRECT, 5},
        {opcodes::JSR, 6},
        {opcodes::RTS, 6},

        // Branches (2 +1 if taken, +1 more if the target is on another page)
        {opcodes::BCC, 2},
        {opcodes::BCS, 2},
        {opcodes::BEQ, 2},
        {opcodes::BMI, 2},
        {opcodes::BNE, 2},
        {opcodes::BPL, 2},
        {opcodes::BVC, 2},
        {opcodes::BVS, 2},

        // Status flag changes
        {opcodes::CLC, 2},
        {opcodes::CLD, 2},
        {opcodes::CLI, 2},
        {opcodes::CLV, 2},
        {opcodes::SEC, 2},
        {opcodes::SED, 2},
        {opcodes::SEI, 2},

        // System
        {opcodes::BRK, 7},
        {opcodes::NOP, 2},
        {opcodes::RTI, 6}
    };

}

#endif