#include "components/Memory.h"
#include "components/Cpu.h"
#include "assembler/Assembler.h"
#include "testing/TestCPU.h"
#include "testing/MainTester.h"
#include <string>

void loadTestProgram(Memory &memory)
{
    memory[0x200] = opcodes::LDA;
    memory[0x201] = (u32)0x42;

    memory[0x202] = opcodes::LDX;
    memory[0x203] = (u32)0x32;
}

int main(int argc, char *argv[])
{
    Memory memory;
    CPU cpu;
    cpu.reset(memory);

    Assembler::newCompileAssemblyProgram(cpu, memory);
    cpu.exec(memory);
    return 0;
};