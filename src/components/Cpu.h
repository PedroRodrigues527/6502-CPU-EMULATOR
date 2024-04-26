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
    Byte fetchByte(s32 &ClockCycles, Memory &memory);
    Word fetchWord(s32 &ClockCycles, Memory &memory);
    Byte readByte(s32 &ClockCycles, Memory &memory, Byte address);
    Word addAddresses(Word address1, Word address2, s32 &ClockCycles);
    Byte getAcc();
    Byte getRX();
    Byte getRY();
    void setAcc(Byte value);
    void setRX(Byte value);
    void setRY(Byte value);
    void decrementClockCycles(s32 &ClockCycles);
    void LOAD_flag_processing(Byte value);
    void exec(s32 ClockCycles, Memory &memory);
    void setProgramCounter(Word value);
    void setStackPointer(Word value);
    void setProcessorFlags(int flag, bool value);
};

#endif
