#include "Cpu.h"
#include <iostream>

void CPU::incrementProgramCounter()
{
    ProgramCounter++;
}

void CPU::reset(Memory &memory)
{
    ProgramCounter = 0xfffc; // [0x0200, 0xffff]
    StackPointer = 0x0100;   // [0x0100, 0x01ff]
    // ProcessorStatus is initialized as [0,0,0,0,0,0,0,0]
    Acc = RX = RY = 0;

    memory.init();
}

Byte CPU::fetchByte(s32 &ClockCycles, Memory &memory)
{
    Byte Data = memory[ProgramCounter];
    incrementProgramCounter();
    ClockCycles--;
    return Data;
}

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

Byte CPU::readByte(s32 &ClockCycles, Memory &memory, Byte address)
{
    Byte Data = memory[address];
    ClockCycles--;
    return Data;
}

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
            Acc = value;
            LOAD_flag_processing(value);
            break;
        }
        case opcodes::LDA_ZERO_PAGE:
        {
            Byte address = fetchByte(ClockCycles, memory);
            Acc = readByte(ClockCycles, memory, address);
            LOAD_flag_processing(Acc);
            break;
        }
        case opcodes::LDA_ZERO_PAGE_X:
        {
            Byte address = fetchByte(ClockCycles, memory);
            address += RX;
            ClockCycles--;
            // TODO: verify when address overflows
            Acc = readByte(ClockCycles, memory, address);
            LOAD_flag_processing(Acc);
            break;
        }
        case opcodes::LDX:
        {
            Byte value = fetchByte(ClockCycles, memory);
            RX = value;
            LOAD_flag_processing(value);
            break;
        }
        case opcodes::LDX_ZERO_PAGE:
        {
            Byte address = fetchByte(ClockCycles, memory);
            RX = readByte(ClockCycles, memory, address);
            LOAD_flag_processing(RX);
            break;
        }
        case opcodes::LDY:
        {
            Byte value = fetchByte(ClockCycles, memory);
            RY = value;
            LOAD_flag_processing(value);
            break;
        }
        case opcodes::LDY_ZERO_PAGE:
        {
            Byte address = fetchByte(ClockCycles, memory);
            RY = readByte(ClockCycles, memory, address);
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
            memory.writeByte(Acc, addr, ClockCycles);
            break;
        }
        case opcodes::STA_ZERO_PAGE_X:
        {
            Word address_to_store_acc = fetchWord(ClockCycles, memory);
            memory.writeByte(Acc, address_to_store_acc, ClockCycles);
            break;
        }
        default:
            std::cout << "COMMAND NOT FOUND: ";
            std::cout << (int)instruction << "\n";
            break;
        }
    }
    std::cout << "END OF EXECUTION\n";
    
}
