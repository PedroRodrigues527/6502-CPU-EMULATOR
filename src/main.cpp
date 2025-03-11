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

    if (argc >= 2 && std::string(argv[1]) == "--check_errors" && isCPUWithErrors(memory, cpu))
    {
        return EXIT_FAILURE;
    }

    compileAssemblyProgram(cpu, memory);
    return 0;
};