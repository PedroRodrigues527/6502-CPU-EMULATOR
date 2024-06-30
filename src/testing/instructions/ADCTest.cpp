#include <string>
#include "ADCTest.h"

std::string ADCTest::get_identifier()
{
    return "ADC";
}

TestResult ADCTest::immediate_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::zero_page_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::zero_page_x_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::zero_page_y_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::absolute_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::absolute_x_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::absolute_y_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::indirect_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::indirect_x_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::indirect_y_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::acumulator_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::relative_test()
{
    return TestResult::NOT_IMPLEMENTED;
}

TestResult ADCTest::implied_test()
{
    return TestResult::NOT_IMPLEMENTED;
}
