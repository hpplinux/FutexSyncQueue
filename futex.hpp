#include <thread>
#include <atomic>
#include <exception>

class Futex {
    std::atomic<std::thread::id> owner;

public:

    class BadUnlock: public std::exception {
    };

    Futex();

    void lock();

    void unlock();
};


