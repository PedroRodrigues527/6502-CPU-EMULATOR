#include "components/Memory.h"
#include "components/Cpu.h"
#include "testing/TestCPU.h"

#include <string>

/* void loadTestProgram(Memory &memory)
{
    memory[0xfffC] = opcodes::JSR;
    memory[0xfffD] = 0x42;
    memory[0xfffe] = 0x42;
    memory[0x4242] = opcodes::LDA;
    memory[0x4243] = 0x84;
} */

int main(int argc, char *argv[])
{
    Memory memory;
    CPU cpu;
    cpu.reset(memory);

/*     if (isCPUWithErrors(memory, cpu)) 
    {
        return EXIT_FAILURE;
    } */

    if (argc >= 2 && std::string(argv[1]) == "--check_errors")
    {
        if (isCPUWithErrors(memory, cpu)) 
        {
            return EXIT_FAILURE;
        }
    }
    else
    {
        cpu.reset(memory);
        return 0;
    }
    return 0;
};