#include <string>
#include "BaseInstructionTest.h"
#include "../../opcodes/opcodes.h"
#include "../../cycles/cycles.h"

struct LDATest : public BaseInstructionTest {
public:
    using BaseInstructionTest::BaseInstructionTest;
    using BaseInstructionTest::run_test;

    TestResult immediate_test();

protected:
    std::string get_identifier();
};