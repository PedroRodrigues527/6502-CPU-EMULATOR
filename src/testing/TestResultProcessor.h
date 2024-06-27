#pragma once

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
private:
    int mSuccess;
    int mFailure;
    int mNotImplemented;
    int mNotApplicable;
};