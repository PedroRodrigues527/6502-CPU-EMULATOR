#include "components/Memory.h"
#include "components/Cpu.h"
#include "testing/TestCPU.h"
#include "testing/MainTester.h"

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

    if (argc >= 2 && std::string(argv[1]) == "--check_errors")
    {
        MainTester tester(memory, cpu);
        tester.execute();
        return 0;
    }
    else
    {
        cpu.reset(memory);
        return 0;
    }
    return 0;
};