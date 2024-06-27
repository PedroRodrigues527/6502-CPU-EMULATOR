#include <string>
#include "LDATest.h"

std::string LDATest::get_identifier()
{
    return "LDA";
}

TestResult LDATest::immediate_test()
{
    mMemory[0x0200] = opcodes::LDA;
    mMemory[0x0201] = 0x42;

    mCPU.exec(cycles::LOAD_CYCLES, mMemory);
    return mCPU.getAcc() == 0x42 ? TestResult::SUCCESS : TestResult::FAILURE;
}

TestResult LDATest::zero_page_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult LDATest::zero_page_x_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult LDATest::zero_page_y_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult LDATest::absolute_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult LDATest::absolute_x_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult LDATest::absolute_y_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult LDATest::indirect_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult LDATest::indirect_x_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult LDATest::indirect_y_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult LDATest::acumulator_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult LDATest::relative_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult LDATest::implied_test()
{
    return TestResult::NOT_IMPLEMENTED;
}
