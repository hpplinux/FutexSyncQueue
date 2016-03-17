#include <algorithm>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "futex.hpp"


template <class Mutex>
void increment(int &counter, int &selfCounter, uint32_t iterations, Mutex &futex) {
    for (uint32_t i = 0; i < iterations; ++i) {
        futex.lock();
        ++counter;
        futex.unlock();

        ++selfCounter;
    }
}

template <class Mutex>
void runThreads(uint64_t iterations, uint64_t threadCount) {
    std::cerr << "iterations: " << iterations << "\nthreads: " << threadCount << "\n";

    auto start = std::chrono::system_clock().now();

    int counter = 0;
    std::vector<std::thread> threads;
    std::vector<int> selfCounter(threadCount, 0);

    Mutex futex;
    for (uint32_t i = 0; i < threadCount; ++i)
        threads.emplace_back(increment<Mutex>, std::ref(counter), std::ref(selfCounter[i]), iterations, std::ref(futex));
    for (auto &thread: threads)
        thread.join();

    int checkSum = std::accumulate(selfCounter.begin(), selfCounter.end(), 0);
    assert(checkSum == counter);

    auto end = std::chrono::system_clock().now();
    std::cerr << std::setprecision(3) << 
        "time: " << std::chrono::duration<double>(end - start).count() << "s\n\n";
}

int main() {
    uint32_t hardwareThreads = std::thread::hardware_concurrency();
    const uint32_t iterations = 4e7;

    std::cerr << "Futex:\n\n";
    runThreads<Futex>(iterations, hardwareThreads + 1);
    runThreads<Futex>(iterations * 6, hardwareThreads + 1);

    runThreads<Futex>(iterations, hardwareThreads / 2);
    runThreads<Futex>(iterations, hardwareThreads * 2);

    std::cerr << "std::mutex:\n\n";
    runThreads<std::mutex>(iterations, hardwareThreads / 2);
    runThreads<std::mutex>(iterations, hardwareThreads * 2);
}
