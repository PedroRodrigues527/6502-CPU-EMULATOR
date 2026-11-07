#ifndef CPU_H
#define CPU_H

#define CARRY_FLAG 0
#define ZERO_FLAG 1
#define INTERRUPT_DISABLE 2
#define DECIMAL_MODE_FLAG 3
#define BREAK_COMMAND 4
#define OVERFLOW_FLAG 5
#define NEGATIVE_FLAG 6

#include <bitset>
#include "Memory.h"
#include "../opcodes/opcodes.h"

struct CPU
{
    Word ProgramCounter;
    Word StackPointer; // supports a 256 byte stack located between $0100 and $01FF

    // registers
    Byte Acc;
    Byte RX, RY;

    // flags
    std::bitset<7> ProcessorStatus;

    void incrementProgramCounter();
    void reset(Memory &memory);
    s32  getClockcycles(Byte instruction);
    Byte fetchByte(s32 &ClockCycles, Memory &memory);
    Word fetchWord(s32 &ClockCycles, Memory &memory);
    Byte readByte(s32 &ClockCycles, Memory &memory, Word address);
    Word addAddresses(Word address1, Word address2, s32 &ClockCycles);
    Word getStackPointer();
    bool getProcessorFlag(int flag);
    Byte getStatusByte();
    Byte getAcc();
    Byte getRX();
    Byte getRY();
    void setAcc(Byte value);
    void setRX(Byte value);
    void setRY(Byte value);
    void decrementClockCycles(s32 &ClockCycles);
    Word getProgramCounter();
    void LOAD_flag_processing(Byte value);
    // void exec(Memory &memory);
    s32 exec(Memory &memory);
    void setProgramCounter(Word value);
    void setStackPointer(Word value);
    void setProcessorFlags(int flag, bool value);
    void setStatusByte(Byte status);
    void writeByte(s32 &ClockCycles, Memory &memory, Word address, Byte value);
    void pushByte(s32 &ClockCycles, Memory &memory, Byte value);
    Byte pullByte(s32 &ClockCycles, Memory &memory);
    void pushWord(s32 &ClockCycles, Memory &memory, Word value);
    Word pullWord(s32 &ClockCycles, Memory &memory);
    void addWithCarry(Byte value);
    void subtractWithCarry(Byte value);
    void compare(Byte reg, Byte value);
    void bitTest(Byte value);
    Byte arithmeticShiftLeft(Byte value);
    Byte logicalShiftRight(Byte value);
    Byte rotateLeft(Byte value);
    Byte rotateRight(Byte value);
    void branch(bool condition, s32 &ClockCycles, Memory &memory);

};

#endif
