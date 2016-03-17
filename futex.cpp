#include <thread>
#include <atomic>
#include <exception>
#include "futex.hpp"

Futex::Futex(): owner(std::thread::id(0)) {
}

void Futex::lock() {
    auto thisId = std::this_thread::get_id();
    auto zero = std::thread::id(0);
    auto expected = zero;

    while (!owner.compare_exchange_weak(expected, thisId)) {
        expected = zero;
        std::this_thread::yield();
    }
}

void Futex::unlock() {
    auto thisId = std::this_thread::get_id();
    auto zero = std::thread::id(0);

    if (!owner.compare_exchange_weak(thisId, zero))
        throw new BadUnlock;
}
