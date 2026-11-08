#include "Cpu.h"
#include <iostream>

namespace
{
    /**
     * Every instruction ("what to do") of the official 6502 instruction set.
     */
    enum class Mnemonic
    {
        ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
        CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP,
        JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI,
        RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA
    };

    /**
     * Every addressing mode ("where is the operand").
     */
    enum class AddressingMode
    {
        IMPLIED, ACCUMULATOR, IMMEDIATE, ZERO_PAGE, ZERO_PAGE_X, ZERO_PAGE_Y,
        ABSOLUTE, ABSOLUTE_X, ABSOLUTE_Y, INDIRECT, INDIRECT_X, INDIRECT_Y, RELATIVE
    };

    struct Decoded
    {
        Mnemonic mnemonic;
        AddressingMode mode;
        bool valid;
    };

// DECODE(opcode constant, instruction, addressing mode)
#define DECODE(code, mnemonic, mode) \
    case opcodes::code: return {Mnemonic::mnemonic, AddressingMode::mode, true};

// The 8 "classic" modes shared by ADC, AND, CMP, EOR, LDA, ORA and SBC
#define DECODE_ALU(name)                                  \
    DECODE(name, name, IMMEDIATE)                         \
    DECODE(name##_ZERO_PAGE, name, ZERO_PAGE)             \
    DECODE(name##_ZERO_PAGE_X, name, ZERO_PAGE_X)         \
    DECODE(name##_ABSOLUTE, name, ABSOLUTE)               \
    DECODE(name##_ABSOLUTE_X, name, ABSOLUTE_X)           \
    DECODE(name##_ABSOLUTE_Y, name, ABSOLUTE_Y)           \
    DECODE(name##_INDIRECT_X, name, INDIRECT_X)           \
    DECODE(name##_INDIRECT_Y, name, INDIRECT_Y)

// The 5 modes shared by ASL, LSR, ROL and ROR
#define DECODE_SHIFT(name)                                \
    DECODE(name##_ACCUMULATOR, name, ACCUMULATOR)         \
    DECODE(name##_ZERO_PAGE, name, ZERO_PAGE)             \
    DECODE(name##_ZERO_PAGE_X, name, ZERO_PAGE_X)         \
    DECODE(name##_ABSOLUTE, name, ABSOLUTE)               \
    DECODE(name##_ABSOLUTE_X, name, ABSOLUTE_X)

    /**
     * Translates an opcode to its instruction and addressing mode.
     * @param Byte opcode
     * @returns Decoded - valid is false for opcodes that are not part of the official set
     */
    Decoded decode(Byte opcode)
    {
        switch (opcode)
        {
            DECODE_ALU(ADC)
            DECODE_ALU(AND)
            DECODE_ALU(CMP)
            DECODE_ALU(EOR)
            DECODE_ALU(LDA)
            DECODE_ALU(ORA)
            DECODE_ALU(SBC)

            DECODE_SHIFT(ASL)
            DECODE_SHIFT(LSR)
            DECODE_SHIFT(ROL)
            DECODE_SHIFT(ROR)

            DECODE(LDX, LDX, IMMEDIATE)
            DECODE(LDX_ZERO_PAGE, LDX, ZERO_PAGE)
            DECODE(LDX_ZERO_PAGE_Y, LDX, ZERO_PAGE_Y)
            DECODE(LDX_ABSOLUTE, LDX, ABSOLUTE)
            DECODE(LDX_ABSOLUTE_Y, LDX, ABSOLUTE_Y)

            DECODE(LDY, LDY, IMMEDIATE)
            DECODE(LDY_ZERO_PAGE, LDY, ZERO_PAGE)
            DECODE(LDY_ZERO_PAGE_X, LDY, ZERO_PAGE_X)
            DECODE(LDY_ABSOLUTE, LDY, ABSOLUTE)
            DECODE(LDY_ABSOLUTE_X, LDY, ABSOLUTE_X)

            DECODE(STA_ZERO_PAGE, STA, ZERO_PAGE)
            DECODE(STA_ZERO_PAGE_X, STA, ZERO_PAGE_X)
            DECODE(STA_ABSOLUTE, STA, ABSOLUTE)
            DECODE(STA_ABSOLUTE_X, STA, ABSOLUTE_X)
            DECODE(STA_ABSOLUTE_Y, STA, ABSOLUTE_Y)
            DECODE(STA_INDIRECT_X, STA, INDIRECT_X)
            DECODE(STA_INDIRECT_Y, STA, INDIRECT_Y)

            DECODE(STX_ZERO_PAGE, STX, ZERO_PAGE)
            DECODE(STX_ZERO_PAGE_Y, STX, ZERO_PAGE_Y)
            DECODE(STX_ABSOLUTE, STX, ABSOLUTE)

            DECODE(STY_ZERO_PAGE, STY, ZERO_PAGE)
            DECODE(STY_ZERO_PAGE_X, STY, ZERO_PAGE_X)
            DECODE(STY_ABSOLUTE, STY, ABSOLUTE)

            DECODE(CPX, CPX, IMMEDIATE)
            DECODE(CPX_ZERO_PAGE, CPX, ZERO_PAGE)
            DECODE(CPX_ABSOLUTE, CPX, ABSOLUTE)

            DECODE(CPY, CPY, IMMEDIATE)
            DECODE(CPY_ZERO_PAGE, CPY, ZERO_PAGE)
            DECODE(CPY_ABSOLUTE, CPY, ABSOLUTE)

            DECODE(BIT_ZERO_PAGE, BIT, ZERO_PAGE)
            DECODE(BIT_ABSOLUTE, BIT, ABSOLUTE)

            DECODE(INC_ZERO_PAGE, INC, ZERO_PAGE)
            DECODE(INC_ZERO_PAGE_X, INC, ZERO_PAGE_X)
            DECODE(INC_ABSOLUTE, INC, ABSOLUTE)
            DECODE(INC_ABSOLUTE_X, INC, ABSOLUTE_X)

            DECODE(DEC_ZERO_PAGE, DEC, ZERO_PAGE)
            DECODE(DEC_ZERO_PAGE_X, DEC, ZERO_PAGE_X)
            DECODE(DEC_ABSOLUTE, DEC, ABSOLUTE)
            DECODE(DEC_ABSOLUTE_X, DEC, ABSOLUTE_X)

            DECODE(JMP_ABSOLUTE, JMP, ABSOLUTE)
            DECODE(JMP_INDIRECT, JMP, INDIRECT)

            DECODE(BCC, BCC, RELATIVE)
            DECODE(BCS, BCS, RELATIVE)
            DECODE(BEQ, BEQ, RELATIVE)
            DECODE(BMI, BMI, RELATIVE)
            DECODE(BNE, BNE, RELATIVE)
            DECODE(BPL, BPL, RELATIVE)
            DECODE(BVC, BVC, RELATIVE)
            DECODE(BVS, BVS, RELATIVE)

            DECODE(JSR, JSR, ABSOLUTE)
            DECODE(RTS, RTS, IMPLIED)
            DECODE(RTI, RTI, IMPLIED)
            DECODE(BRK, BRK, IMPLIED)
            DECODE(NOP, NOP, IMPLIED)

            DECODE(CLC, CLC, IMPLIED)
            DECODE(CLD, CLD, IMPLIED)
            DECODE(CLI, CLI, IMPLIED)
            DECODE(CLV, CLV, IMPLIED)
            DECODE(SEC, SEC, IMPLIED)
            DECODE(SED, SED, IMPLIED)
            DECODE(SEI, SEI, IMPLIED)

            DECODE(INX, INX, IMPLIED)
            DECODE(INY, INY, IMPLIED)
            DECODE(DEX, DEX, IMPLIED)
            DECODE(DEY, DEY, IMPLIED)

            DECODE(TAX, TAX, IMPLIED)
            DECODE(TAY, TAY, IMPLIED)
            DECODE(TXA, TXA, IMPLIED)
            DECODE(TYA, TYA, IMPLIED)
            DECODE(TSX, TSX, IMPLIED)
            DECODE(TXS, TXS, IMPLIED)

            DECODE(PHA, PHA, IMPLIED)
            DECODE(PHP, PHP, IMPLIED)
            DECODE(PLA, PLA, IMPLIED)
            DECODE(PLP, PLP, IMPLIED)

            default:
                return {Mnemonic::NOP, AddressingMode::IMPLIED, false};
        }
    }

#undef DECODE
#undef DECODE_ALU
#undef DECODE_SHIFT
}

/**
 * Get the value of the accumulator
 * @returns Byte - value of the accumulator
 */
Byte CPU::getAcc()
{
    return Acc;
}

/**
 * Get the value of the X register
 * @returns Byte - value of the X register
 */
Byte CPU::getRX()
{
    return RX;
}

/**
 * Get the value of the Y register
 * @returns Byte - value of the Y register
 */
Byte CPU::getRY()
{
    return RY;
}

/**
 * Set the value of the accumulator
 * @param Byte value
 */
void CPU::setAcc(Byte value)
{
    Acc = value;
}

/**
 * Set the value of the X register
 * @param Byte value
 */
void CPU::setRX(Byte value)
{
    RX = value;
}

/**
 * Set the value of the Y register
 * @param Byte value
 */
void CPU::setRY(Byte value)
{
    RY = value;
}

/**
 * Increment the program counter
 */
void CPU::incrementProgramCounter()
{
    ProgramCounter++;
}

/**
 * Decrement the clock cycles
 * @param s32 &ClockCycles
 */
void CPU::decrementClockCycles(s32 &ClockCycles)
{
    ClockCycles--;
}

/**
 * Get the program counter
 * @returns Word - the program counter
 */
Word CPU::getProgramCounter()
{
    return ProgramCounter;
}

/**
 * Set the program counter
 * @param Word value
 */
void CPU::setProgramCounter(Word value)
{
    ProgramCounter = value;
}

/**
 * Get the stack pointer. It is the full address of the next free stack slot: [0x0100, 0x01ff]
 * @returns Word - the stack pointer
 */
Word CPU::getStackPointer()
{
    return StackPointer;
}

/**
 * Set the stack pointer
 * @param Word value
 */
void CPU::setStackPointer(Word value)
{
    printf("CPU :: setStackPointer\nSP before: %d\n", getStackPointer());
    StackPointer = value;
    printf("CPU :: SP after: %d\n", getStackPointer());
}

void CPU::setProcessorFlags(int flag, bool value = 1)
{
    ProcessorStatus.set(flag, value);
}

/**
 * Get the state of one processor flag
 * @param int flag - CARRY_FLAG, ZERO_FLAG, ...
 * @returns bool - true if the flag is set
 */
bool CPU::getProcessorFlag(int flag)
{
    return ProcessorStatus.test(flag);
}

/**
 * Packs the processor flags into the byte layout of the real 6502 (NV1BDIZC).
 * Bit 5 is unused and always reads as 1.
 * @returns Byte - the processor status
 */
Byte CPU::getStatusByte()
{
    Byte status = 0x20;
    if (ProcessorStatus.test(CARRY_FLAG))             status |= 0x01;
    if (ProcessorStatus.test(ZERO_FLAG))              status |= 0x02;
    if (ProcessorStatus.test(INTERRUPT_DISABLE)) status |= 0x04;
    if (ProcessorStatus.test(DECIMAL_MODE_FLAG))           status |= 0x08;
    if (ProcessorStatus.test(BREAK_COMMAND))             status |= 0x10;
    if (ProcessorStatus.test(OVERFLOW_FLAG))          status |= 0x40;
    if (ProcessorStatus.test(NEGATIVE_FLAG))          status |= 0x80;
    return status;
}

/**
 * Unpacks a status byte (NV1BDIZC) into the processor flags.
 * @param Byte status
 */
void CPU::setStatusByte(Byte status)
{
    setProcessorFlags(CARRY_FLAG,             (status & 0x01) != 0);
    setProcessorFlags(ZERO_FLAG,              (status & 0x02) != 0);
    setProcessorFlags(INTERRUPT_DISABLE, (status & 0x04) != 0);
    setProcessorFlags(DECIMAL_MODE_FLAG,           (status & 0x08) != 0);
    setProcessorFlags(BREAK_COMMAND,             (status & 0x10) != 0);
    setProcessorFlags(OVERFLOW_FLAG,          (status & 0x40) != 0);
    setProcessorFlags(NEGATIVE_FLAG,          (status & 0x80) != 0);
}

/**
 * Resets the memory and all of it's elements.
 * @param Memory &memory
 */
void CPU::reset(Memory &memory)
{
    printf("CPU :: CPU RESET\n");
    setProgramCounter(0x0200); // [0x0200, 0xffff]
    setStackPointer(0x01FF);   // [0x0100, 0x01ff] - the stack grows downwards
    setAcc(0);
    setRX(0);
    setRY(0);
    ProcessorStatus.reset();

    memory.init();
}

void CPU::setDebugMode() {
    inDebug = true;
}

/**
 * Fetch the byte of the current program counter address value.
 * @param s32 &ClockCycles
 * @param Memory &memory
 * @returns data from the memory
 */
Byte CPU::fetchByte(s32 &ClockCycles, Memory &memory)
{
    Byte Data = memory[ProgramCounter];
    incrementProgramCounter();
    decrementClockCycles(ClockCycles);
    return Data;
}

/**
 * Get the clock cycles of the instruction
 * @param Byte instruction
 * @returns s32 - clock cycles of the instruction
 */
s32 CPU::getClockcycles(Byte instruction)
{
    auto it = opcodes::opcode_map.find(instruction);

    if (it != opcodes::opcode_map.end()) {
        return it->second;
    }

    return -1;
}

/**
 * Fetch the word of the current program counter address value.
 * @param s32 &ClockCycles
 * @param Memory &memory
 * @returns data from the memory
 */
Word CPU::fetchWord(s32 &ClockCycles, Memory &memory)
{
    Word Data = fetchByte(ClockCycles, memory);

    Data |= (fetchByte(ClockCycles, memory) << 8);

    return Data;
}

/**
 * Read a byte from any address of the memory (1 clock cycle).
 * @param s32 &ClockCycles
 * @param Memory &memory
 * @param Word address
 * @returns Byte - data from address given
 */
Byte CPU::readByte(s32 &ClockCycles, Memory &memory, Word address)
{
    Byte Data = memory[address];
    decrementClockCycles(ClockCycles);
    return Data;
}

/**
 * Write a byte to any address of the memory (1 clock cycle).
 * @param s32 &ClockCycles
 * @param Memory &memory
 * @param Word address
 * @param Byte value
 */
void CPU::writeByte(s32 &ClockCycles, Memory &memory, Word address, Byte value)
{
    memory[address] = value;
    decrementClockCycles(ClockCycles);
}

/**
 * Push a byte on the stack. The stack lives in [0x0100, 0x01ff] and grows downwards.
 * @param s32 &ClockCycles
 * @param Memory &memory
 * @param Byte value
 */
void CPU::pushByte(s32 &ClockCycles, Memory &memory, Byte value)
{
    writeByte(ClockCycles, memory, StackPointer, value);
    setStackPointer(static_cast<Word>(0x0100 | ((StackPointer - 1) & 0xFF)));
}

/**
 * Pull a byte from the stack.
 * @param s32 &ClockCycles
 * @param Memory &memory
 * @returns Byte - the byte on the top of the stack
 */
Byte CPU::pullByte(s32 &ClockCycles, Memory &memory)
{
    setStackPointer(static_cast<Word>(0x0100 | ((StackPointer + 1) & 0xFF)));
    return readByte(ClockCycles, memory, StackPointer);
}

/**
 * Push a word on the stack, high byte first (so the low byte ends up at the lower address).
 * @param s32 &ClockCycles
 * @param Memory &memory
 * @param Word value
 */
void CPU::pushWord(s32 &ClockCycles, Memory &memory, Word value)
{
    pushByte(ClockCycles, memory, static_cast<Byte>(value >> 8));
    pushByte(ClockCycles, memory, static_cast<Byte>(value & 0xFF));
}

/**
 * Pull a word from the stack, low byte first.
 * @param s32 &ClockCycles
 * @param Memory &memory
 * @returns Word - the word on the top of the stack
 */
Word CPU::pullWord(s32 &ClockCycles, Memory &memory)
{
    Word low = pullByte(ClockCycles, memory);
    Word high = pullByte(ClockCycles, memory);
    return static_cast<Word>(low | (high << 8));
}

/**
 * Add 2 addresses
 * @param Word address1
 * @param Word address2
 * @param s32 ClockCycles
 * @returns Word - sum of the addresses
 */
Word CPU::addAddresses(Word address1, Word address2, s32 &ClockCycles)
{ 
  Word result = address1 + address2;
  decrementClockCycles(ClockCycles);
  return result;
}

/**
 * Sets (or clears) the zero and negative flags according to the value. Used by every
 * instruction that loads, transfers, computes or modifies a byte.
 * @param Byte value
 */
void CPU::LOAD_flag_processing(Byte value)
{
    setProcessorFlags(ZERO_FLAG, value == 0);
    setProcessorFlags(NEGATIVE_FLAG, (value & 0x80) != 0); // last bit is set
}

/**
 * ADC: A = A + value + carry. Supports decimal mode (NMOS 6502 behaviour).
 * @param Byte value
 */
void CPU::addWithCarry(Byte value)
{
    const unsigned a = getAcc();
    const unsigned carry = getProcessorFlag(CARRY_FLAG) ? 1 : 0;
    const unsigned binary = a + value + carry;

    if (!getProcessorFlag(DECIMAL_MODE_FLAG))
    {
        setProcessorFlags(CARRY_FLAG, binary > 0xFF);
        setProcessorFlags(OVERFLOW_FLAG, (~(a ^ value) & (a ^ binary) & 0x80) != 0);
        setAcc(static_cast<Byte>(binary));
        LOAD_flag_processing(getAcc());
        return;
    }

    // Decimal mode: every nibble is a BCD digit
    int low = (a & 0x0F) + (value & 0x0F) + carry;
    if (low >= 0x0A)
    {
        low = ((low + 0x06) & 0x0F) + 0x10;
    }
    int sum = (a & 0xF0) + (value & 0xF0) + low;

    // On the NMOS 6502 N and V come from the intermediate result and Z from the binary one
    setProcessorFlags(NEGATIVE_FLAG, (sum & 0x80) != 0);
    setProcessorFlags(OVERFLOW_FLAG, (~(a ^ value) & (a ^ sum) & 0x80) != 0);
    setProcessorFlags(ZERO_FLAG, (binary & 0xFF) == 0);

    if (sum >= 0xA0)
    {
        sum += 0x60;
    }
    setProcessorFlags(CARRY_FLAG, sum > 0xFF);
    setAcc(static_cast<Byte>(sum & 0xFF));
}

/**
 * SBC: A = A - value - (1 - carry). Supports decimal mode (NMOS 6502 behaviour).
 * @param Byte value
 */
void CPU::subtractWithCarry(Byte value)
{
    const int a = getAcc();
    const int borrow = getProcessorFlag(CARRY_FLAG) ? 0 : 1;
    const int binary = a - value - borrow;

    // On the NMOS 6502 all the flags of SBC come from the binary result, even in decimal mode
    setProcessorFlags(CARRY_FLAG, binary >= 0);
    setProcessorFlags(OVERFLOW_FLAG, ((a ^ value) & (a ^ binary) & 0x80) != 0);
    LOAD_flag_processing(static_cast<Byte>(binary & 0xFF));

    if (!getProcessorFlag(DECIMAL_MODE_FLAG))
    {
        setAcc(static_cast<Byte>(binary & 0xFF));
        return;
    }

    int low = (a & 0x0F) - (value & 0x0F) - borrow;
    if (low < 0)
    {
        low = ((low - 0x06) & 0x0F) - 0x10;
    }
    int result = (a & 0xF0) - (value & 0xF0) + low;
    if (result < 0)
    {
        result -= 0x60;
    }
    setAcc(static_cast<Byte>(result & 0xFF));
}

/**
 * CMP / CPX / CPY: compares a register with a value without changing either.
 * @param Byte reg - value of the register
 * @param Byte value
 */
void CPU::compare(Byte reg, Byte value)
{
    const Byte result = static_cast<Byte>(reg - value);
    setProcessorFlags(CARRY_FLAG, reg >= value);
    setProcessorFlags(ZERO_FLAG, reg == value);
    setProcessorFlags(NEGATIVE_FLAG, (result & 0x80) != 0);
}

/**
 * BIT: Z = (A & value) == 0, V = bit 6 of value, N = bit 7 of value.
 * @param Byte value
 */
void CPU::bitTest(Byte value)
{
    setProcessorFlags(ZERO_FLAG, (getAcc() & value) == 0);
    setProcessorFlags(OVERFLOW_FLAG, (value & 0x40) != 0);
    setProcessorFlags(NEGATIVE_FLAG, (value & 0x80) != 0);
}

/**
 * ASL: shifts left, bit 7 goes to the carry and bit 0 is cleared.
 * @param Byte value
 * @returns Byte - shifted value
 */
Byte CPU::arithmeticShiftLeft(Byte value)
{
    setProcessorFlags(CARRY_FLAG, (value & 0x80) != 0);
    Byte result = static_cast<Byte>(value << 1);
    LOAD_flag_processing(result);
    return result;
}

/**
 * LSR: shifts right, bit 0 goes to the carry and bit 7 is cleared.
 * @param Byte value
 * @returns Byte - shifted value
 */
Byte CPU::logicalShiftRight(Byte value)
{
    setProcessorFlags(CARRY_FLAG, (value & 0x01) != 0);
    Byte result = static_cast<Byte>(value >> 1);
    LOAD_flag_processing(result);
    return result;
}

/**
 * ROL: rotates left through the carry.
 * @param Byte value
 * @returns Byte - rotated value
 */
Byte CPU::rotateLeft(Byte value)
{
    const Byte oldCarry = getProcessorFlag(CARRY_FLAG) ? 1 : 0;
    setProcessorFlags(CARRY_FLAG, (value & 0x80) != 0);
    Byte result = static_cast<Byte>((value << 1) | oldCarry);
    LOAD_flag_processing(result);
    return result;
}

/**
 * ROR: rotates right through the carry.
 * @param Byte value
 * @returns Byte - rotated value
 */
Byte CPU::rotateRight(Byte value)
{
    const Byte oldCarry = getProcessorFlag(CARRY_FLAG) ? 0x80 : 0;
    setProcessorFlags(CARRY_FLAG, (value & 0x01) != 0);
    Byte result = static_cast<Byte>((value >> 1) | oldCarry);
    LOAD_flag_processing(result);
    return result;
}

/**
 * Relative branch. Takes 1 extra clock cycle when the branch is taken and another one
 * when the target is on a different page than the next instruction.
 * @param bool condition - true if the branch must be taken
 * @param s32 &ClockCycles
 * @param Memory &memory
 */
void CPU::branch(bool condition, s32 &ClockCycles, Memory &memory)
{
    const signed char offset = static_cast<signed char>(fetchByte(ClockCycles, memory));

    if (!condition)
    {
        return;
    }

    const Word nextInstruction = ProgramCounter;
    setProgramCounter(static_cast<Word>(nextInstruction + offset));
    decrementClockCycles(ClockCycles);

    if ((nextInstruction & 0xFF00) != (ProgramCounter & 0xFF00))
    {
        decrementClockCycles(ClockCycles);
    }
}

s32 CPU::execDebug(Memory &memory) {
    setDebugMode();
    return CPU::exec(memory);
}

/**
 * Brain of the execution of the CPU instructions. Runs the program starting at the
 * program counter until a BRK instruction has been executed (or an unknown opcode is found).
 * @param Memory &memory
 * @returns s32 - the clock cycles used by the program, BRK included
 */
s32 CPU::exec(Memory &memory)
{
    s32 totalCycles = 0;
    s32 ClockCycles = 0; // clock cycles left in the instruction that is being executed
    bool running = true;

    /**
     * Resolves the address the instruction works on, paying the clock cycles of the mode.
     * Indexed modes cost 1 extra cycle when the index crosses a page, or always
     * (alwaysExtraCycle) for instructions that write.
     */
    auto effectiveAddress = [&](AddressingMode mode, bool alwaysExtraCycle) -> Word
    {
        switch (mode)
        {
            case AddressingMode::ZERO_PAGE:
                return fetchByte(ClockCycles, memory);
            case AddressingMode::ZERO_PAGE_X:
            {
                Byte base = fetchByte(ClockCycles, memory);
                decrementClockCycles(ClockCycles); // adding the index takes a cycle
                return static_cast<Byte>(base + getRX()); // wraps around inside the zero page
            }
            case AddressingMode::ZERO_PAGE_Y:
            {
                Byte base = fetchByte(ClockCycles, memory);
                decrementClockCycles(ClockCycles);
                return static_cast<Byte>(base + getRY());
            }
            case AddressingMode::ABSOLUTE:
                return fetchWord(ClockCycles, memory);
            case AddressingMode::ABSOLUTE_X:
            case AddressingMode::ABSOLUTE_Y:
            {
                Word base = fetchWord(ClockCycles, memory);
                Word address = static_cast<Word>(base + (mode == AddressingMode::ABSOLUTE_X ? getRX() : getRY()));
                if (alwaysExtraCycle || (base & 0xFF00) != (address & 0xFF00))
                {
                    decrementClockCycles(ClockCycles);
                }
                return address;
            }
            case AddressingMode::INDIRECT_X:
            {
                Byte pointer = fetchByte(ClockCycles, memory);
                decrementClockCycles(ClockCycles);
                pointer = static_cast<Byte>(pointer + getRX());
                Word low = readByte(ClockCycles, memory, pointer);
                Word high = readByte(ClockCycles, memory, static_cast<Byte>(pointer + 1)); // pointer wraps in the zero page
                return static_cast<Word>(low | (high << 8));
            }
            case AddressingMode::INDIRECT_Y:
            {
                Byte pointer = fetchByte(ClockCycles, memory);
                Word low = readByte(ClockCycles, memory, pointer);
                Word high = readByte(ClockCycles, memory, static_cast<Byte>(pointer + 1));
                Word base = static_cast<Word>(low | (high << 8));
                Word address = static_cast<Word>(base + getRY());
                if (alwaysExtraCycle || (base & 0xFF00) != (address & 0xFF00))
                {
                    decrementClockCycles(ClockCycles);
                }
                return address;
            }
            default:
                return 0;
        }
    };

    // Reads the operand of the instruction (immediate value or the byte in memory)
    auto readOperand = [&](AddressingMode mode) -> Byte
    {
        if (mode == AddressingMode::IMMEDIATE)
        {
            return fetchByte(ClockCycles, memory);
        }
        Word address = effectiveAddress(mode, false);
        return readByte(ClockCycles, memory, address);
    };

    // The operation of the read-modify-write instructions
    auto modifyValue = [&](Mnemonic mnemonic, Byte value) -> Byte
    {
        switch (mnemonic)
        {
            case Mnemonic::ASL: return arithmeticShiftLeft(value);
            case Mnemonic::LSR: return logicalShiftRight(value);
            case Mnemonic::ROL: return rotateLeft(value);
            case Mnemonic::ROR: return rotateRight(value);
            case Mnemonic::INC:
            {
                Byte result = static_cast<Byte>(value + 1);
                LOAD_flag_processing(result);
                return result;
            }
            case Mnemonic::DEC:
            {
                Byte result = static_cast<Byte>(value - 1);
                LOAD_flag_processing(result);
                return result;
            }
            default:
                return value;
        }
    };

    // ASL, LSR, ROL, ROR, INC and DEC: read the value, write it back, write the new value
    auto readModifyWrite = [&](Mnemonic mnemonic, AddressingMode mode)
    {
        if (mode == AddressingMode::ACCUMULATOR)
        {
            decrementClockCycles(ClockCycles);
            setAcc(modifyValue(mnemonic, getAcc()));
            return;
        }
        Word address = effectiveAddress(mode, true);
        Byte value = readByte(ClockCycles, memory, address);
        decrementClockCycles(ClockCycles); // the 6502 writes the old value back before the new one
        writeByte(ClockCycles, memory, address, modifyValue(mnemonic, value));
    };

    while (running)
    {
        const Byte opcode = memory[ProgramCounter];
        const s32 instructionCycles = getClockcycles(opcode);
        const Decoded instruction = decode(opcode);

        if (instructionCycles < 0 || !instruction.valid)
        {
            std::cout << "COMMAND NOT FOUND: " << std::hex << (int)opcode
                      << " (at address " << ProgramCounter << ")" << std::dec << "\n";
            break;
        }

        ClockCycles = instructionCycles;
        fetchByte(ClockCycles, memory); // the opcode itself

        switch (instruction.mnemonic)
        {
            // ------------------------------------------------------ Loads
            case Mnemonic::LDA:
            {
                Byte value = readOperand(instruction.mode);
                setAcc(value);
                LOAD_flag_processing(value);
                break;
            }
            case Mnemonic::LDX:
            {
                Byte value = readOperand(instruction.mode);
                setRX(value);
                LOAD_flag_processing(value);
                break;
            }
            case Mnemonic::LDY:
            {
                Byte value = readOperand(instruction.mode);
                setRY(value);
                LOAD_flag_processing(value);
                break;
            }

            // ----------------------------------------------------- Stores
            case Mnemonic::STA:
            {
                Word address = effectiveAddress(instruction.mode, true);
                writeByte(ClockCycles, memory, address, getAcc());
                break;
            }
            case Mnemonic::STX:
            {
                Word address = effectiveAddress(instruction.mode, true);
                writeByte(ClockCycles, memory, address, getRX());
                break;
            }
            case Mnemonic::STY:
            {
                Word address = effectiveAddress(instruction.mode, true);
                writeByte(ClockCycles, memory, address, getRY());
                break;
            }

            // ------------------------------------------ Register transfers
            case Mnemonic::TAX:
                decrementClockCycles(ClockCycles);
                setRX(getAcc());
                LOAD_flag_processing(getRX());
                break;
            case Mnemonic::TAY:
                decrementClockCycles(ClockCycles);
                setRY(getAcc());
                LOAD_flag_processing(getRY());
                break;
            case Mnemonic::TXA:
                decrementClockCycles(ClockCycles);
                setAcc(getRX());
                LOAD_flag_processing(getAcc());
                break;
            case Mnemonic::TYA:
                decrementClockCycles(ClockCycles);
                setAcc(getRY());
                LOAD_flag_processing(getAcc());
                break;
            case Mnemonic::TSX:
                decrementClockCycles(ClockCycles);
                setRX(static_cast<Byte>(StackPointer & 0xFF));
                LOAD_flag_processing(getRX());
                break;
            case Mnemonic::TXS: // does not touch the flags
                decrementClockCycles(ClockCycles);

                printf("CPU :: TXS BEFORE: SP: %d\n", getStackPointer());
                setStackPointer(static_cast<Word>(0x0100 | getRX()));
                printf("CPU :: TXS AFTER: SP: %d\n", getStackPointer());


                break;

            // ----------------------------------------------------- Stack
            case Mnemonic::PHA:
                decrementClockCycles(ClockCycles);
                pushByte(ClockCycles, memory, getAcc());
                break;
            case Mnemonic::PHP: // the pushed copy always has the break flag set
                decrementClockCycles(ClockCycles);
                pushByte(ClockCycles, memory, static_cast<Byte>(getStatusByte() | 0x10));
                break;
            case Mnemonic::PLA:
            {
                decrementClockCycles(ClockCycles);
                decrementClockCycles(ClockCycles); // the stack pointer is incremented
                setAcc(pullByte(ClockCycles, memory));
                LOAD_flag_processing(getAcc());
                break;
            }
            case Mnemonic::PLP:
                decrementClockCycles(ClockCycles);
                decrementClockCycles(ClockCycles);
                setStatusByte(pullByte(ClockCycles, memory));
                break;

            // --------------------------------------------------- Logical
            case Mnemonic::AND:
            {
                Byte value = readOperand(instruction.mode);
                setAcc(getAcc() & value);
                LOAD_flag_processing(getAcc());
                break;
            }
            case Mnemonic::EOR:
            {
                Byte value = readOperand(instruction.mode);
                setAcc(getAcc() ^ value);
                LOAD_flag_processing(getAcc());
                break;
            }
            case Mnemonic::ORA:
            {
                Byte value = readOperand(instruction.mode);
                setAcc(getAcc() | value);
                LOAD_flag_processing(getAcc());
                break;
            }
            case Mnemonic::BIT:
                bitTest(readOperand(instruction.mode));
                break;

            // ------------------------------------------------ Arithmetic
            case Mnemonic::ADC:
                addWithCarry(readOperand(instruction.mode));
                break;
            case Mnemonic::SBC:
                subtractWithCarry(readOperand(instruction.mode));
                break;
            case Mnemonic::CMP:
                compare(getAcc(), readOperand(instruction.mode));
                break;
            case Mnemonic::CPX:
                compare(getRX(), readOperand(instruction.mode));
                break;
            case Mnemonic::CPY:
                compare(getRY(), readOperand(instruction.mode));
                break;

            // ------------------------------- Increments, decrements, shifts
            case Mnemonic::INC:
            case Mnemonic::DEC:
            case Mnemonic::ASL:
            case Mnemonic::LSR:
            case Mnemonic::ROL:
            case Mnemonic::ROR:
                readModifyWrite(instruction.mnemonic, instruction.mode);
                break;
            case Mnemonic::INX:
                decrementClockCycles(ClockCycles);
                setRX(static_cast<Byte>(getRX() + 1));
                LOAD_flag_processing(getRX());
                break;
            case Mnemonic::INY:
                decrementClockCycles(ClockCycles);
                setRY(static_cast<Byte>(getRY() + 1));
                LOAD_flag_processing(getRY());
                break;
            case Mnemonic::DEX:
                decrementClockCycles(ClockCycles);
                setRX(static_cast<Byte>(getRX() - 1));
                LOAD_flag_processing(getRX());
                break;
            case Mnemonic::DEY:
                decrementClockCycles(ClockCycles);
                setRY(static_cast<Byte>(getRY() - 1));
                LOAD_flag_processing(getRY());
                break;

            // ------------------------------------------------------ Jumps
            case Mnemonic::JMP:
            {
                if (instruction.mode == AddressingMode::ABSOLUTE)
                {
                    setProgramCounter(fetchWord(ClockCycles, memory));
                    break;
                }
                // Indirect: the original 6502 never carries into the high byte of the pointer,
                // so a pointer at $xxFF takes its high byte from $xx00.
                Word pointer = fetchWord(ClockCycles, memory);
                Word low = readByte(ClockCycles, memory, pointer);
                Word high = readByte(ClockCycles, memory, static_cast<Word>((pointer & 0xFF00) | ((pointer + 1) & 0x00FF)));
                setProgramCounter(static_cast<Word>(low | (high << 8)));
                break;
            }
            case Mnemonic::JSR:
            {
                Word subRoutineAddress = fetchWord(ClockCycles, memory);
                pushWord(ClockCycles, memory, static_cast<Word>(ProgramCounter - 1)); // address of the last byte of the JSR
                setProgramCounter(subRoutineAddress);
                decrementClockCycles(ClockCycles);
                break;
            }
            case Mnemonic::RTS:
            {
                decrementClockCycles(ClockCycles);
                decrementClockCycles(ClockCycles);
                Word returnAddress = pullWord(ClockCycles, memory);
                setProgramCounter(static_cast<Word>(returnAddress + 1));
                decrementClockCycles(ClockCycles);
                break;
            }
            case Mnemonic::RTI:
            {
                decrementClockCycles(ClockCycles);
                decrementClockCycles(ClockCycles);
                setStatusByte(pullByte(ClockCycles, memory));
                setProgramCounter(pullWord(ClockCycles, memory));
                break;
            }

            // --------------------------------------------------- Branches
            case Mnemonic::BCC: branch(!getProcessorFlag(CARRY_FLAG),    ClockCycles, memory); break;
            case Mnemonic::BCS: branch(getProcessorFlag(CARRY_FLAG),     ClockCycles, memory); break;
            case Mnemonic::BEQ: branch(getProcessorFlag(ZERO_FLAG),      ClockCycles, memory); break;
            case Mnemonic::BNE: branch(!getProcessorFlag(ZERO_FLAG),     ClockCycles, memory); break;
            case Mnemonic::BMI: branch(getProcessorFlag(NEGATIVE_FLAG),  ClockCycles, memory); break;
            case Mnemonic::BPL: branch(!getProcessorFlag(NEGATIVE_FLAG), ClockCycles, memory); break;
            case Mnemonic::BVS: branch(getProcessorFlag(OVERFLOW_FLAG),  ClockCycles, memory); break;
            case Mnemonic::BVC: branch(!getProcessorFlag(OVERFLOW_FLAG), ClockCycles, memory); break;

            // ------------------------------------------- Status changes
            case Mnemonic::CLC: decrementClockCycles(ClockCycles); setProcessorFlags(CARRY_FLAG, false); break;
            case Mnemonic::SEC: decrementClockCycles(ClockCycles); setProcessorFlags(CARRY_FLAG, true); break;
            case Mnemonic::CLD: decrementClockCycles(ClockCycles); setProcessorFlags(DECIMAL_MODE_FLAG, false); break;
            case Mnemonic::SED: decrementClockCycles(ClockCycles); setProcessorFlags(DECIMAL_MODE_FLAG, true); break;
            case Mnemonic::CLI: decrementClockCycles(ClockCycles); setProcessorFlags(INTERRUPT_DISABLE, false); break;
            case Mnemonic::SEI: decrementClockCycles(ClockCycles); setProcessorFlags(INTERRUPT_DISABLE, true); break;
            case Mnemonic::CLV: decrementClockCycles(ClockCycles); setProcessorFlags(OVERFLOW_FLAG, false); break;

            // ----------------------------------------------------- System
            case Mnemonic::NOP:
                decrementClockCycles(ClockCycles);
                break;
            case Mnemonic::BRK:
            {
                fetchByte(ClockCycles, memory); // BRK is followed by a padding byte that is skipped
                pushWord(ClockCycles, memory, ProgramCounter);
                pushByte(ClockCycles, memory, static_cast<Byte>(getStatusByte() | 0x10));
                setProcessorFlags(INTERRUPT_DISABLE, true);
                setProcessorFlags(BREAK_COMMAND, true);
                Word low = readByte(ClockCycles, memory, 0xFFFE);
                Word high = readByte(ClockCycles, memory, 0xFFFF);
                setProgramCounter(static_cast<Word>(low | (high << 8)));
                running = false; // the emulator hands control back after a BRK
                break;
            }
        }

        totalCycles += instructionCycles - ClockCycles;
    }

    return totalCycles;
}