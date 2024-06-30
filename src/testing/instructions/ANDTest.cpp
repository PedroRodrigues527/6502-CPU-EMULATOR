#include <string>
#include "ANDTest.h"

std::string ANDTest::get_identifier()
{
    return "AND";
}

TestResult ANDTest::immediate_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::zero_page_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::zero_page_x_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::zero_page_y_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::absolute_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::absolute_x_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::absolute_y_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::indirect_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::indirect_x_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::indirect_y_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::acumulator_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::relative_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ANDTest::implied_test()
{
    return TestResult::NOT_IMPLEMENTED;
}
