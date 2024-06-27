#pragma once
#include <string>
#include <iostream>

#define RESET  "\033[0m"
#define BLACK  "\033[30m"
#define RED    "\033[31m" 
#define GREEN  "\033[32m"
#define YELLOW "\033[33m"  
#define CYAN   "\033[36m"  

enum TestResult {
    SUCCESS = 0,
    FAILURE = 1,
    NOT_IMPLEMENTED = 2,
    NOT_APPLICABLE = 3
};

struct TestResultProcessor
{
public:
    TestResultProcessor() {};

    void process(TestResult result, std::string identifier);
private:
    int mSuccess;
    int mFailure;
    int mNotImplemented;
    int mNotApplicable;

    void output_error();
    void output_success();
    void output_not_implemented();
    void output_not_applicable();
};