#pragma once
#include <string>
#include "../../components/Memory.h"
#include "../../components/Cpu.h"
#include <iostream>
#include "TestResult.h"

#define RESET  "\033[0m"
#define BLACK  "\033[30m"
#define RED    "\033[31m" 
#define GREEN  "\033[32m"
#define YELLOW "\033[33m"  
#define CYAN   "\033[36m"  

struct BaseInstructionTest
{
public:
    BaseInstructionTest(Memory &memory, CPU &CPU);
    
    virtual TestResult immediate_test() = 0;

    int run_test();
protected:
    virtual std::string get_identifier() = 0;
    Memory mMemory;
    CPU mCPU;
private:
    void process_mode(TestResult result, std::string mode_identifier);
    void output_error();
    void output_success();
    void output_not_implemented();
    void output_not_applicable();
};