#include "Cpu.h"
#include <iostream>

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
 * Resets the memory and all of it's elements.
 * @param Memory &memory
 * @returns data from the memory
*/
void CPU::reset(Memory &memory)
{
    ProgramCounter = 0xfffc; // [0x0200, 0xffff]
    StackPointer = 0x0100;   // [0x0100, 0x01ff]
    setAcc(0);
    setRX(0);
    setRY(0);
    ProcessorStatus.reset();

    memory.init();
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
    ClockCycles--;
    return Data;
}

/**
 * Fetch the word of the current program counter address value.
 * @param s32 &ClockCycles
 * @param Memory &memory
 * @returns data from the memory
*/
Word CPU::fetchWord(s32 &ClockCycles, Memory &memory)
{
    Word Data = memory[ProgramCounter];
    incrementProgramCounter();
    ClockCycles--;

    Data |= (memory[ProgramCounter] << 8);
    incrementProgramCounter();
    ClockCycles--;

    return Data;
}

/**
 * Add 2 addresses
 * @param s32 &ClockCycles
 * @param Memory &memory
 * @param Byte address
 * @returns Byte - data from address given
*/
Byte CPU::readByte(s32 &ClockCycles, Memory &memory, Byte address)
{
    Byte Data = memory[address];
    ClockCycles--;
    return Data;
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
    ClockCycles--;
    Word result = address1 + address2;
    return result;
}

/**
 * Set's the bit set corresponding flag, according to the value, for LOAD operations.
 * @param Byte value
*/
void CPU::LOAD_flag_processing(Byte value)
{
    if (value == 0)
    {
        ProcessorStatus.set(ZERO_FLAG, 1);
    }
    if (value & (1 << 7)) // if last bit is set
    {
        ProcessorStatus.set(NEGATIVE_FLAG, 1);
    }
}

/**
 * Brain of the execution of the CPU instructions.
 * @param s32 ClockCycles
 * @param Memory &memory
 * @returns Word - sum of the addresses
*/
void CPU::exec(s32 ClockCycles, Memory &memory)
{
    while (ClockCycles > 0)
    {
        Byte instruction = fetchByte(ClockCycles, memory);
        switch (instruction)
        {
        case opcodes::LDA:
        {
            Byte value = fetchByte(ClockCycles, memory);
            setAcc(value);
            LOAD_flag_processing(value);
            break;
        }
        case opcodes::LDA_ZERO_PAGE:
        {
            Byte address = fetchByte(ClockCycles, memory);
            setAcc(readByte(ClockCycles, memory, address));
            LOAD_flag_processing(Acc);
            break;
        }
        case opcodes::LDA_ZERO_PAGE_X:
        {
            Byte address = fetchByte(ClockCycles, memory);
            address += getRX();
            ClockCycles--;
            // TODO: verify when address overflows
            setAcc(readByte(ClockCycles, memory, address));
            LOAD_flag_processing(Acc);
            break;
        }
        case opcodes::LDX:
        {
            Byte value = fetchByte(ClockCycles, memory);
            setRX(value);
            LOAD_flag_processing(value);
            break;
        }
        case opcodes::LDX_ZERO_PAGE:
        {
            Byte address = fetchByte(ClockCycles, memory);
            setRX(readByte(ClockCycles, memory, address));
            LOAD_flag_processing(RX);
            break;
        }
        case opcodes::LDY:
        {
            Byte value = fetchByte(ClockCycles, memory);
            setRY(value);
            LOAD_flag_processing(value);
            break;
        }
        case opcodes::LDY_ZERO_PAGE:
        {
            Byte address = fetchByte(ClockCycles, memory);
            setRY(readByte(ClockCycles, memory, address));
            LOAD_flag_processing(RX);
            break;
        }
        case opcodes::JSR:
        {
            Word sub_routine_address = fetchWord(ClockCycles, memory);
            memory.writeWord(ProgramCounter - 1, StackPointer++, ClockCycles);
            ProgramCounter = sub_routine_address;
            ClockCycles--;
            break;
        }
        case opcodes::STA_ZERO_PAGE:
        {
            Word addr = fetchByte(ClockCycles, memory);
            memory.writeByte(getAcc(), addr, ClockCycles);
            break;
        }
        case opcodes::STA_ZERO_PAGE_X:
        {
            Byte address_from_instruction = fetchByte(ClockCycles, memory);
            Word address_to_store_acc = addAddresses(address_from_instruction, getRX(), ClockCycles);
            memory.writeByte(getAcc(), address_to_store_acc, ClockCycles);
            break;
        }
        case opcodes::STA_ZERO_PAGE_ABSOLUTE:
        {
            Word address_to_store_acc = fetchWord(ClockCycles, memory);
            memory.writeByte(getAcc(), address_to_store_acc, ClockCycles);
            break;
        }
        case opcodes::STA_ZERO_PAGE_ABSOLUTE_X:
        {
            Word address_from_instruction = fetchWord(ClockCycles, memory);
            Word address_to_store_acc = addAddresses(address_from_instruction, getRX(), ClockCycles);
            memory.writeByte(getAcc(), address_to_store_acc, ClockCycles);
            break;
        }
        default:
            std::cout << "COMMAND NOT FOUND: ";
            std::cout << (int)instruction << "\n";
            break;
        }
    }
}
