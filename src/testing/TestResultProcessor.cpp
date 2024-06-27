#include "TestResultProcessor.h"

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
            output_success();
            break;
        case TestResult::FAILURE:
            output_error();
            break;
        case TestResult::NOT_IMPLEMENTED:
            output_not_implemented();
            break;
        case TestResult::NOT_APPLICABLE:
            output_not_applicable();
            break;
    }
}