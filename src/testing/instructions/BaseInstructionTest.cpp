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

void BaseInstructionTest::output_not_implemented()
{
    std::cout << YELLOW << "NOT IMPLEMENTED" << RESET << std::endl;
}

void BaseInstructionTest::output_not_applicable()
{
    std::cout << CYAN << "NOT APPLICABLE" << RESET << std::endl;
}

void BaseInstructionTest::process_mode(TestResult result, std::string mode_identifier)
{
    std::cout << get_identifier() + " - " + mode_identifier + " test - ";

    switch (result) {
        case TestResult::SUCCESS:
            output_success();
            break;
        case TestResult::FAILURE:
            output_error();
            break;
        case TestResult::NOT_IMPLEMENTED:
            output_not_implemented();
        case TestResult::NOT_APPLICABLE:
            output_not_applicable();
            break;
    }
    mCPU.reset(mMemory);
}