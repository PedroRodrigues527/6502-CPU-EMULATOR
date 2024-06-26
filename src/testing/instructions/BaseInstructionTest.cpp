#include <string>
#include "BaseInstructionTest.h"

BaseInstructionTest::BaseInstructionTest(Memory &memory, CPU &CPU) {
    mMemory = memory;
    mCPU = CPU;
}

int BaseInstructionTest::run_test()
{
    process_mode(immediate_test(), "immediate mode");
    return 0;
}

void BaseInstructionTest::output_error()
{
    std::cout << RED << "ERROR" << RESET << std::endl;
}

void BaseInstructionTest::output_success()
{
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
}

void BaseInstructionTest::process_mode(int result, std::string mode_identifier)
{
    std::cout << get_identifier() + " - " + mode_identifier + " test - ";
    result ? output_success() : output_error();
    mCPU.reset(mMemory);
}