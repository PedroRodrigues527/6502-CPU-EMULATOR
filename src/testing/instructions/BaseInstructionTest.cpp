#include <string>
#include "BaseInstructionTest.h"

BaseInstructionTest::BaseInstructionTest(Memory &memory, CPU &CPU, TestResultProcessor testProcessor)
{
    mMemory = memory;
    mCPU = CPU;
    mTestProcessor = testProcessor;
}

int BaseInstructionTest::run_test()
{
    process_mode(immediate_test(), "immediate mode");
    process_mode(zero_page_test(), "zero page mode");
    process_mode(zero_page_x_test(), "zero page x mode");
    process_mode(zero_page_y_test(), "zero page y mode");
    process_mode(absolute_test(), "absolute mode");
    process_mode(absolute_x_test(), "absolute x mode");
    process_mode(absolute_y_test(), "absolute y mode");
    process_mode(indirect_test(), "indirect mode");
    process_mode(indirect_x_test(), "indirect x mode");
    process_mode(indirect_y_test(), "indirect y mode");
    process_mode(acumulator_test(), "acumulator mode");
    process_mode(relative_test(), "relative mode");
    process_mode(implied_test(), "implied mode");

    return 0;
}

void BaseInstructionTest::process_mode(TestResult result, std::string mode_identifier)
{
    mTestProcessor.process(result, get_identifier() + " " + mode_identifier);
    mCPU.reset(mMemory);
}