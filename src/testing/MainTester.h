#include <vector>
#include "instructions/LDATest.h"
#include "instructions/BaseInstructionTest.h"
#include "../components/Memory.h"
#include "../components/Cpu.h"

struct MainTester
{
public:
    MainTester(Memory &memory, CPU &CPU);
    void execute();

private:
    void init_instructions_tests();
    Memory mMemory;
    CPU mCPU;
    std::vector<BaseInstructionTest*> mTests;
};