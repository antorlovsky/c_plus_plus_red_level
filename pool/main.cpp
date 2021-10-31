#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>

using namespace std;

template <class T>
class ObjectPool {
public:
    T* Allocate() {
        if (!pool_.empty()) {
            T* object = pool_.front();

            pool_.pop();
            allocated_.insert(object);

            return object;
        }
        else {
            T* object = new T();
            allocated_.insert(object);

            return object;
        }
    }

    T* TryAllocate() {
        if (!pool_.empty()) {
            T* object = pool_.front();

            pool_.pop();
            allocated_.insert(object);

            return object;
        }
        else {
            return nullptr;
        }
    }

    void Deallocate(T* object) {
        if (allocated_.count(object) == 0)
            throw invalid_argument("Wrong deallocate of object");

        allocated_.erase(object);
        pool_.push(object);
    }

    ~ObjectPool() {
        while (!pool_.empty()) {
            delete pool_.front();
            pool_.pop();
        }

        for (auto& item : allocated_)
            delete item;
    }

private:
    queue<T*> pool_;
    set<T*> allocated_;
};

void TestObjectPool() {
    ObjectPool<string> pool;

    auto p1 = pool.Allocate();
    auto p2 = pool.Allocate();
    auto p3 = pool.Allocate();

    *p1 = "first";
    *p2 = "second";
    *p3 = "third";

    pool.Deallocate(p2);
    ASSERT_EQUAL(*pool.Allocate(), "second");

    pool.Deallocate(p3);
    pool.Deallocate(p1);
    ASSERT_EQUAL(*pool.Allocate(), "third");
    ASSERT_EQUAL(*pool.Allocate(), "first");

    pool.Deallocate(p1);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestObjectPool);
    return 0;
}