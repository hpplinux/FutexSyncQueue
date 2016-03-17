#include <queue>
#include <exception>
#include "futex.hpp"


template<typename T>
class ISyncQueue {

public:

    ISyncQueue() {
    }

    virtual ~ISyncQueue() {
    }

    virtual void lock() = 0;

    virtual void unlock() = 0;

    virtual size_t size() = 0;

    virtual bool empty() = 0;

    virtual void push(const T&) = 0;

    virtual void pop() = 0;

    virtual T& front() = 0;
};

template<typename T>
class SyncQueue: public ISyncQueue<T> {
    std::queue<T> queue;
    Futex futex;

public:

    SyncQueue() {
    }

    virtual ~SyncQueue() {
    }

    class BadPopException: public std::exception {
    };

    void lock() override {
        futex.lock();
    }

    void unlock() override {
        futex.unlock();
    }

    size_t size() override {
        return queue.size();
    }

    bool empty() override {
        return queue.empty();
    }

    void push(const T &element) override {
        queue.push(element);
    }

    void pop() override {
        if (queue.empty())
            throw new SyncQueue<T>::BadPopException();
        queue.pop();
    }

    T &front() override {
        return queue.front();
    }
};
