#include <string>
#include "BaseInstructionTest.h"
#include "../../opcodes/opcodes.h"

struct LDATest : public BaseInstructionTest {
public:
    using BaseInstructionTest::BaseInstructionTest;
    using BaseInstructionTest::run_test;

    bool immediate_test();

protected:
    std::string get_identifier();
};