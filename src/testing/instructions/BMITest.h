#include <string>
#include "../BaseInstructionTest.h"
#include "../../opcodes/opcodes.h"
#include "../../cycles/cycles.h"

struct BMITest : public BaseInstructionTest {
public:
    using BaseInstructionTest::BaseInstructionTest;
    using BaseInstructionTest::run_test;

    TestResult immediate_test();
    TestResult zero_page_test();
    TestResult zero_page_x_test();
    TestResult zero_page_y_test();
    TestResult absolute_test();
    TestResult absolute_x_test();
    TestResult absolute_y_test();
    TestResult indirect_test();
    TestResult indirect_x_test();
    TestResult indirect_y_test();
    TestResult acumulator_test();
    TestResult relative_test();
    TestResult implied_test();

protected:
    std::string get_identifier();
};