#include "MainTester.h"
#include "TestResultProcessor.h"

MainTester::MainTester(Memory &memory, CPU &CPU) {
    mMemory = memory;
    mCPU = CPU;
}

void MainTester::init_instructions_tests()
{
    mTests.push_back(new LDATest(mMemory, mCPU, mTestProcessor));
}

void MainTester::execute()
{
    init_instructions_tests();

    for (auto& test: mTests) {
        test->run_test();
    }

    mTestProcessor.outputResult();
}