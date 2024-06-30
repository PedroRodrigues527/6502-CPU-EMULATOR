#include <vector>
#include "instructions/ADCTest.h"
#include "instructions/ANDTest.h"
#include "instructions/LDATest.h"
#include "BaseInstructionTest.h"
#include "../components/Memory.h"
#include "../components/Cpu.h"
#include "TestResultProcessor.h"

struct MainTester
{
public:
    MainTester(Memory &memory, CPU &CPU);
    void execute();

private:
    void init_instructions_tests();
    Memory mMemory;
    CPU mCPU;
    TestResultProcessor mTestProcessor;
    std::vector<BaseInstructionTest*> mTests;
};