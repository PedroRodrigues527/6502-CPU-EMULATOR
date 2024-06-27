#include <string>
#include "BaseInstructionTest.h"

BaseInstructionTest::BaseInstructionTest(Memory &memory, CPU &CPU, TestResultProcessor &testProcessor) {
    mMemory = memory;
    mCPU = CPU;
    mTestProcessor = testProcessor;
}

int BaseInstructionTest::run_test()
{
    process_mode(immediate_test(), "immediate mode");
    return 0;
}

void BaseInstructionTest::process_mode(TestResult result, std::string mode_identifier)
{
    mTestProcessor.process(result, get_identifier() + " " + mode_identifier);
    mCPU.reset(mMemory);
}