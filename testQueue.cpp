#include "queue.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <random>

class QueueTest: public ::testing::TestWithParam<uint32_t> {
public:
};

void pushThread(SyncQueue<int> &queue, std::vector<int> &data) {
    for (int element: data) {
        queue.lock();
        queue.push(element);
        queue.unlock();
    }
}

void popThread(SyncQueue<int> &queue, std::vector<int> &data) {
    for (int &element: data) {
        queue.lock();
        element = queue.front();
        queue.pop();
        queue.unlock();
    }
}

void sizeCheckThread(SyncQueue<int> &queue, uint32_t expected) {
    queue.lock();
    ASSERT_EQ(queue.size(), expected);
    queue.unlock();
}

void emptyCheckThread(SyncQueue<int> &queue) {
    queue.lock();
    ASSERT_EQ(queue.empty(), true);
    queue.unlock();
}

TEST_P(QueueTest, PushPopSizeFrontEmptyTest) {
    uint32_t size = 1e6;
    uint32_t threadCount = GetParam();
    std::mt19937 generator;

    std::vector<std::thread> threads;
    std::vector<std::vector<int>> data(threadCount, std::vector<int>(size));
    std::vector<int> all, result;
    SyncQueue<int> queue;

    for (uint32_t i = 0; i < threadCount; ++i) {
        for (uint32_t j = 0; j < size; ++j) {
            data[i][j] = generator();
            all.push_back(data[i][j]);
        }
        threads.emplace_back(pushThread, std::ref(queue), std::ref(data[i]));
    }
    for (auto &thread: threads)
        thread.join();

    threads.clear();
    for (uint32_t i = 0; i < threadCount; ++i)
        threads.emplace_back(sizeCheckThread, std::ref(queue), size * threadCount);
    for (auto &thread: threads)
        thread.join();

    threads.clear();
    for (uint32_t i = 0; i < threadCount; ++i)
        threads.emplace_back(popThread, std::ref(queue), std::ref(data[i]));
    for (auto &thread: threads)
        thread.join();

    threads.clear();
    for (uint32_t i = 0; i < threadCount; ++i)
        threads.emplace_back(emptyCheckThread, std::ref(queue));
    for (auto &thread: threads)
        thread.join();

    for (uint32_t i = 0; i < threadCount; ++i)
        for (uint32_t j = 0; j < size; ++j)
            result.push_back(data[i][j]);
    std::sort(all.begin(), all.end());
    std::sort(result.begin(), result.end());
    ASSERT_EQ(all, result);
}

INSTANTIATE_TEST_CASE_P(AllOperationsTest, QueueTest,
            ::testing::Values(1, 4, 9, 16, 100));

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
