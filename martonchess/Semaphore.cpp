
#include "Semaphore.h"

Semaphore::Semaphore()
: free(true) {
}

void Semaphore::acquire() {
    std::unique_lock<std::mutex> lock(mutex);
    while (!free) {
        condition.wait(lock);
    }
    free = false;
}

void Semaphore::release() {
    std::unique_lock<std::mutex> lock(mutex);
    free = true;
    condition.notify_one();
}
