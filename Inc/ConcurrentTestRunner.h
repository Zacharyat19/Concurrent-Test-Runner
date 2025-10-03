#ifndef CONCURRENT_TEST_RUNNER_H
#define CONCURRENT_TEST_RUNNER_H

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <map>
#include <functional>
#include <chrono>

// Represents a single test case
struct TestCase {
    std::string name;        // test name
    std::string command;     // shell command or executable to run
    int timeoutSeconds;      // max allowed runtime
};

// Represents the result of a test case
struct TestResult {
    std::string name;
    bool passed;
    int exitCode;
    double runtime;          // seconds
    std::string output;      // captured stdout/stderr
};

class TestQueue {
public:
    void push(const TestCase& test);
    bool pop(TestCase& test);
    bool empty() const;

private:
    std::queue<TestCase> queue_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
};

class ConcurrentTestRunner {
public:
    ConcurrentTestRunner(int numThreads);
    ~ConcurrentTestRunner();

    void addTest(const TestCase& test);
    void run();
    std::vector<TestResult> getResults() const;

private:
    void workerThread();  // function executed by each worker
    TestResult executeTest(const TestCase& test);

    int numThreads_;
    bool stop_;
    std::vector<std::thread> workers_;
    TestQueue taskQueue_;
    std::vector<TestResult> results_;
    mutable std::mutex resultsMtx_;
};

#endif // CONCURRENT_TEST_RUNNER_H
