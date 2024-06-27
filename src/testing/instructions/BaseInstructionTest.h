#pragma once
#include "../../components/Memory.h"
#include "../../components/Cpu.h"
#include "../TestResultProcessor.h"

struct BaseInstructionTest
{
public:
    BaseInstructionTest(Memory &memory, CPU &CPU, TestResultProcessor testProcessor);
    
    virtual TestResult immediate_test() = 0;
    virtual TestResult zero_page_test() = 0;
    virtual TestResult zero_page_x_test() = 0;
    virtual TestResult zero_page_y_test() = 0;
    virtual TestResult absolute_test() = 0;
    virtual TestResult absolute_x_test() = 0;
    virtual TestResult absolute_y_test() = 0;
    virtual TestResult indirect_test() = 0;
    virtual TestResult indirect_x_test() = 0;
    virtual TestResult indirect_y_test() = 0;
    virtual TestResult acumulator_test() = 0;
    virtual TestResult relative_test() = 0;
    virtual TestResult implied_test() = 0;

    int run_test();
protected:
    virtual std::string get_identifier() = 0;
    Memory mMemory;
    CPU mCPU;
    TestResultProcessor mTestProcessor;
private:
    void process_mode(TestResult result, std::string mode_identifier);
};