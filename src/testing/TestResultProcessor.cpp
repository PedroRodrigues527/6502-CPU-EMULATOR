#include "TestResultProcessor.h"

int TestResultProcessor::mSuccess = 0;
int TestResultProcessor::mFailure = 0;
int TestResultProcessor::mNotImplemented = 0;
int TestResultProcessor::mNotApplicable = 0;

void TestResultProcessor::output_error()
{
    std::cout << RED << "ERROR" << RESET << std::endl;
}

void TestResultProcessor::output_success()
{
    std::cout << GREEN << "SUCCESS" << RESET << std::endl;
}

void TestResultProcessor::output_not_implemented()
{
    std::cout << YELLOW << "NOT IMPLEMENTED" << RESET << std::endl;
}

void TestResultProcessor::output_not_applicable()
{
    std::cout << CYAN << "NOT APPLICABLE" << RESET << std::endl;
}

void TestResultProcessor::process(TestResult result, std::string identifier)
{
    std::cout << identifier + " test - ";
    switch (result) {
        case TestResult::SUCCESS:
            TestResultProcessor::mSuccess++;
            output_success();
            break;
        case TestResult::FAILURE:
            TestResultProcessor::mFailure++;
            output_error();
            break;
        case TestResult::NOT_IMPLEMENTED:
            TestResultProcessor::mNotImplemented++;
            output_not_implemented();
            break;
        case TestResult::NOT_APPLICABLE:
            TestResultProcessor::mNotApplicable++;
            output_not_applicable();
            break;
    }
}

void TestResultProcessor::outputResult()
{
    std::cout << "Success: " << TestResultProcessor::mSuccess << std::endl;
    std::cout << "Failure: " << TestResultProcessor::mFailure << std::endl;
    std::cout << "Not implemented: " << TestResultProcessor::mNotImplemented << std::endl;
    std::cout << "Not applicable: " << TestResultProcessor::mNotApplicable << std::endl;


}