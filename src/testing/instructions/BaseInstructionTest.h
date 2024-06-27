#pragma once
#include "../../components/Memory.h"
#include "../../components/Cpu.h"
#include "../TestResultProcessor.h"

struct BaseInstructionTest
{
public:
    BaseInstructionTest(Memory &memory, CPU &CPU, TestResultProcessor &testProcessor);
    
    virtual TestResult immediate_test() = 0;

    int run_test();
protected:
    virtual std::string get_identifier() = 0;
    
    Memory mMemory;
    CPU mCPU;
    TestResultProcessor mTestProcessor;private:
    void process_mode(TestResult result, std::string mode_identifier);
};