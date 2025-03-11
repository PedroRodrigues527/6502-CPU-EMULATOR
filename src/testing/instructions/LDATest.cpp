#include <string>
#include "LDATest.h"

std::string LDATest::get_identifier()
{
    return "LDA";
}

bool LDATest::immediate_test()
{
    mMemory[0x0200] = opcodes::LDA;
    mMemory[0x0201] = 0x42;

    mCPU.exec(mMemory);
    return mCPU.getAcc() == 0x42;
};